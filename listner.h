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

#ifndef LISTNER_H
#define LISTNER_H

#include <vector>
#include <cstdint>
#include <ev.h>
#include <thread>
#include <exception>
#include "worker.h"

namespace listnerClass {

/*
class bindEx : public std::exception {
	virtual const char* what() const throw() {
		return "Can't bind to socket, socket already in use";
	}
} bindException;
*/

class listner {
public:
	listner(struct ev_loop *loop);
	~listner();

	void ioAccept(ev_io *io, int events);


private:
	struct ev_loop 			*m_loop;
	ev_io				m_io;
	int				m_s;
	uint64_t			m_connections;
	uint16_t			m_workers;
	std::vector<worker *>		m_workerQueue;
	std::vector<std::thread *>	m_threads;
	/* std::vector<tcpConnection *>	m_connectionObjectPool;*/
	bool				m_debug;
};

}

#endif // LISTNER_H
