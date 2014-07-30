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

#include <iostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <time.h>
#include <getopt.h>
#include <ev.h>
#include "listner.h"
#include "log.h"
#include "configmanager.h"

int main(int argc, char **argv) {
	try {
		configManager *cfg = configManager::getInstance();
		int c;
		int debug = 0;
		int trace = 0;
		int dummy = 0;
		while (1) {
			static struct option long_options[] = {
				{"debug",	no_argument,		&debug, 1},
				{"trace",	no_argument,		&trace, 1},
				{"dummy",	no_argument,		&dummy, 1},
				{"port",	no_argument,		0, 'p'},
				{"workers",	no_argument,		0, 'w'},
				{"cachetime",	no_argument,		0, 'c'},
				{"sleeptime",	no_argument,		0, 's'},
				{"qeuesize",	no_argument,		0, 'q'},
				{"help",	no_argument,		0, 'h'},
				{0, 0, 0, 0}
				};
			int option_index = 0;

			c = getopt_long (argc, argv, "p:w:c:s:q:", long_options, &option_index);
			if (c == -1)
				break;

			switch (c) {
				case 0:
					/* If this option set a flag, do nothing else now. */
					if (long_options[option_index].flag != 0)
						break;
				case 'p':
					cfg->setListenPort(boost::lexical_cast<uint16_t>(optarg));
					break;
				case 'w':
					cfg->setWorkersCount(boost::lexical_cast<uint16_t>(optarg));
					break;
				case 'c':
					cfg->setCacheTime(boost::lexical_cast<uint16_t>(optarg));
					break;
				case 's':
					cfg->setThreadSleepTime(boost::lexical_cast<uint16_t>(optarg));
					break;
				case 'q':
					cfg->setDBWriteSize(boost::lexical_cast<uint32_t>(optarg));
					break;
				case 'h':
					std::cout<<"Usage:"<<std::endl
						 <<"--port       -p    -- listning port"<<std::endl
						 <<"--workers    -w    -- number of workers"<<std::endl
						 <<"--cachtime   -c    -- time to cache data, in ms"<<std::endl
						 <<"--sleeptime  -s    -- time to sleep betwen worker's iterations, in ms"<<std::endl
						 <<"--queuesize  -q    -- maximum queue size inside worker, before send"<<std::endl
						 <<"--debug            -- enable debug messages"<<std::endl
						 <<"--trace            -- enable trace messages (more verbose then debug)"<<std::endl;
						exit(0);
				default:
					abort();
			}
		}

		if (debug) {
			cfg->setLogLevel(boost::log::trivial::debug);
			cfg->setDebug(static_cast<bool>(debug));
		} else if (trace) {
			cfg->setLogLevel(boost::log::trivial::trace);
		} else {
			cfg->setLogLevel(boost::log::trivial::info);
		}

		cfg->setDummy(dummy);

		initLog();
		struct ev_loop *loop = EV_DEFAULT;
		listnerClass::listner listner(loop);
		ev_run(loop, 0);
		std::cout<<"Error initializing loop"<<std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
