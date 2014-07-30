/*
 * Copyright 2014 Vladimir Smirnov <civil.over@gmail.com>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#include <cstdint>
#include <iostream>
#include <thread>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include "configmanager.h"
#include "common.h"
#include "worker.h"
#include "log.h"
#include "dbclientdata.h"
#include "influxdbwriter.h"

//#define TRACET BOOST_LOG_TRIVIAL(trace)<<" ["<<std::setw(3)<<m_threadNumber<<"]"<<VERB_FMT
#define ERRORT BOOST_LOG_TRIVIAL(error)<<" ["<<std::setw(3)<<m_threadNumber<<"]"<<VERB_FMT
#define DEBUGT BOOST_LOG_TRIVIAL(debug)<<" ["<<std::setw(3)<<m_threadNumber<<"]"<<VERB_FMT
//#define DEBUGT std::cout<<std::endl<<" ["<<std::setw(3)<<m_threadNumber<<"]"<<VERB_FMT

void genericCallbackBridge(struct ev_loop *loop, ev_io *w, int revents)
{
	myIo *io = reinterpret_cast<myIo *>(w);
	io->currentThread->readData(loop, io, revents);
}

worker::worker(uint16_t threadNumber)
	: m_threadNumber(threadNumber)
{
	m_debug = configManager::getInstance()->getDebug();
	m_needDump = false;
	m_iterations = 0;
	for (auto i = 0; i < backlog; i++) {
		m_io[i] = new myIo;
		m_io[i]->currentThread = this;
		memset(m_io[i]->buffer, 0, 1024);
		m_io[i]->buffer[0] = 0;
		m_ioQueue.push_back(m_io[i]);
	}

	for (uint32_t i = 0; i < configManager::getInstance()->getDataQueueSize(); i++) {
		struct graphData *data = new graphData;
		m_objectCache.push_back(data);
	}

	m_loop = ev_loop_new(EVFLAG_AUTO);
	if (m_debug) {
		DEBUGT<<"Worker started";
	}
//	ev_set_timeout_collect_interval(0.01);
//	ev_set_io_collect_interval(0.01);
//	TRACET<<"Created";
}

void worker::readData (struct ev_loop *loop, myIo *io, int events)
{
	int spaces[2] = {0, 0};
	int spaceCnt = 0;
	int i = 0;
	char *src = nullptr;
	ssize_t len = 0;
	ssize_t prev = 0;
	ev_io_stop(loop, (ev_io *)io);
	if (m_debug) {
		DEBUGT<<"Got event, event code: "<<events;
	}

	if (events & EV_ERROR) {
		ERRORT<<"got invalid event";
		return;
	}

	if (events & EV_READ) {
		len = recv(io->io.fd, io->readBuffer, sizeof(io->readBuffer), 0);
		if (m_debug) {
			DEBUGT<<"Received "<<len<<" bytes";
		}

//		TRACET<<"Received some data: "<<len<< " bytes";
		if (len == 0) {
			close(io->io.fd);
			ev_io_stop(loop, (ev_io *)io);
			m_ioQueue.push_back(io);
			return;
		}

		/* Parse message from graphite */
		for (i = 0; i < len; i++) {
			if (io->readBuffer[i] == ' ') {
				spaceCnt++;
				if (spaceCnt > 2) {
					continue;
				}
				spaces[spaceCnt - 1] = i + 1;
				io->readBuffer[i] = 0;
				continue;
			} else if (io->readBuffer[i] != '\n') {
				continue;
			}

			/* We've ran out of objectCache, breaking loop */
			if (m_objectCache.empty()) {
				if (m_debug) {
					DEBUGT<<"No object cache left";
				}

				break;
			}

			/* Skipping broken and empty metrics */
			if ((spaceCnt == 0) || (spaceCnt > 2)) {
				prev = i;
				continue;
			}

			io->readBuffer[i] = 0;
			if ((prev != 0) || (io->buffer[0] == 0)) {
				src = io->readBuffer;
			} else {
				size_t strLen = strlen(io->buffer);
				strcpy(io->buffer + strLen, io->readBuffer);
				src = io->buffer;
			}

			if ((src + prev)[0] == '\n') {
				src++;
			}

			struct graphData *data = m_objectCache.back();
			m_objectCache.pop_back();
			strcpy(data->name, src + prev);
			data->data = atof(src + spaces[0]);
			data->timestamp = atoi(src + spaces[1]);

			if (data->name[0] == '\n') {
				ERROR<<"strange data for metric, prev="<<prev<<" src="<<(uint64_t)src<<" spaces[0]="<<spaces[0]<<" spaces[1]="<<spaces[1]<<" i="<<i<<" spaceCnt="<<spaceCnt;
			}

			prev = i + 1;
			if (data->name[0] != 0) {
				m_outputBuffer.push_back(data);
			}

			spaceCnt = 0;
		}

//		if ((io->readBuffer+prev)[0] == '\n') {
//			prev++;
//		}

		/* Store data for next iteration */
		if (i - prev != 0) {
			memcpy(io->buffer, io->readBuffer + prev, i - prev);
			io->buffer[i - prev] = 0;
		} else {
			io->buffer[0] = 0;
		}
	} else {
		ERRORT<<"Unknown event: "<<events;
		ev_io_stop(loop, (ev_io *)io);
		return;
	}

	ev_io_start(loop, (ev_io *)io);
}



void worker::processQueue()
{
	int sd;

	abstractDBClient *writer = reinterpret_cast<abstractDBClient *>(new influxdbWriter());

	uint16_t threadSleepTime = configManager::getInstance()->getThreadSleepTime();
	uint16_t cacheTime = configManager::getInstance()->getCacheTime();
	uint32_t writeQueueSize = configManager::getInstance()->getDBWriteSize();
	uint32_t outputQueueSize = 0;
	uint16_t iterationThreshold = cacheTime/threadSleepTime;
	myIo *io = nullptr;

//	TRACET<<"Processing queue...";
	while(1) {
////		TRACET<<"iteration started";
		while(queue.pop(sd)) {
			if (m_ioQueue.empty()) {
				if (m_debug) {
					DEBUGT<<"ioQueue is empty, waiting for events to complete...";
				}
				break;
			}

			io = m_ioQueue.back();
			m_ioQueue.pop_back();
			ev_io_init((ev_io *)io, &genericCallbackBridge, sd, EV_READ);
			ev_io_start(m_loop, (ev_io *)io);
		}

////		TRACET<<"iteration done";
		m_iterations++;
		outputQueueSize = m_outputBuffer.size();
//		if (m_debug) {
//			DEBUGT<<"Iteration #: "<<m_iterations;
//		}
		if ((((m_iterations > iterationThreshold) || (outputQueueSize >= writeQueueSize)) && (outputQueueSize > 0 )) || (m_objectCache.empty())) {
			if (m_debug) {
				DEBUGT<<"Inserting...";
			}

			writer->insertData(m_outputBuffer, m_objectCache);
			m_needDump = false;
			m_iterations = 0;
		}

		ev_run(m_loop, EVRUN_NOWAIT);
		std::this_thread::sleep_for(std::chrono::milliseconds(threadSleepTime));
	}
}

worker::~worker()
{
	for (auto i = 0; i < backlog; i ++) {
		delete m_io[i];
	}

	for (uint32_t i = 0; i < configManager::getInstance()->getDataQueueSize(); i++) {
		struct graphData *data = m_objectCache.back();
		m_objectCache.pop_back();
		delete data;
	}
}
