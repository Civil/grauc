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

#ifndef WORKER_H
#define WORKER_H

#include <boost/lockfree/spsc_queue.hpp>
#include <ev.h>
#include "dbclientdata.h"

struct myIo;
const uint16_t backlog = 16*1024;

class worker
{
public:
	
	void processQueue();
	void readData(struct ev_loop *loop, myIo *io, int events);
	worker(uint16_t threadNumber);
	~worker();

	boost::lockfree::spsc_queue<int, boost::lockfree::capacity<backlog>> queue;
private:
	struct ev_loop 			*m_loop;
	myIo				*m_io[backlog];
	uint16_t			m_threadNumber;
	bool				m_needDump;
	uint32_t			m_iterations;
	std::vector<myIo *>		m_ioQueue;
	std::vector<struct graphData *>	m_outputBuffer;
	std::vector<struct graphData *>	m_objectCache;
	bool				m_debug;
};

struct myIo {
	ev_io io;
	int fd;
	worker *currentThread;
	char readBuffer[8*1024];
	char buffer[1024];
};

#endif // WORKER_H
