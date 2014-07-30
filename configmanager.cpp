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

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "configmanager.h"

configManager::configManager()
{
	setDataQueueSize(1024*1024);
	setListenPort(2024);
	setWorkersCount(4);
	setDBPort(8086);
	setDBHost("http://mtrsgr01t.yandex.ru");
	setDBUrl("/db/graphite/series?u=graphite&p=graphite");
	setCacheTime(1000);
	setThreadSleepTime(50);
	setDBWriteSize(100000);
	setLogLevel(boost::log::trivial::info);
	setDummy(false);
	setDebug(false);
}

configManager::~configManager()
{

}

std::string configManager::getDBURL()
{
	return m_DBHost + ":" + boost::lexical_cast<std::string>(m_DBPort) + m_DBUrl;
}


configManager *configManager::instance = nullptr;
