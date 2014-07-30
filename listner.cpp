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

#include <ctime>
#include <ev.h>
#include <thread>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "log.h"
#include "listner.h"
#include "configmanager.h"

namespace listnerClass {

static listner *currentInstance = nullptr;

void inline ioAcceptBridge(struct ev_loop *loop, ev_io *io, int events) {
	std::ignore = loop;
	currentInstance->ioAccept(io, events);
}

listner::listner(struct ev_loop *loop)
{
	uint16_t port = configManager::getInstance()->getListenPort();
	struct sockaddr_in6 addr;
	m_workers = configManager::getInstance()->getWorkersCount();
	m_debug = configManager::getInstance()->getDebug();
	m_connections = 0;
	m_loop = loop;
	int status;
	int yes = 1;
	currentInstance = this;

	for (uint16_t i = 0; i < m_workers; i++) {
		m_workerQueue.push_back(new worker(i));
		m_threads.push_back(new std::thread(&worker::processQueue, m_workerQueue.back()));
	}
	
	m_s = socket(PF_INET6, SOCK_STREAM, 0);

	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_addr = in6addr_any;

	if ((status = setsockopt(m_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) != 0) {
		INFO<<"Failed to set socket options, code: "<<status<<" msg: "<<strerror(errno);
	}
	

	if ((status = bind(m_s, (struct sockaddr *)&addr, sizeof(addr))) != 0) {
		INFO<<"Failed to bind to port "<< port << " code: "<<status << " msg: " << strerror(errno);
		throw;
	}

	fcntl(m_s, F_SETFL, fcntl(m_s, F_GETFL, 0) | O_NONBLOCK); 
	listen(m_s, 5);

	ev_io_init(&m_io, &ioAcceptBridge, m_s, EV_READ);
	ev_io_start(m_loop, &m_io);
	INFO<<"Server started";
}

listner::~listner()
{

}

void listner::ioAccept(ev_io *io, int events)
{
	if (EV_ERROR & events) {
//		TRACE<<"got invalid event";
		return;
	}
	m_connections++;

	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int clientSD = accept(io->fd, (struct sockaddr *)&clientAddr, &clientLen);

	if (clientSD < 0) {
		if (m_debug) {
			DEBUG<<"Accept failed, code: "<<clientSD;
		}
		return;
	}

	worker *workerThread = m_workerQueue[m_connections % m_workers];
	while(!workerThread->queue.push(clientSD));
}

}
