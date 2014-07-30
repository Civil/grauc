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

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <boost/log/trivial.hpp>
#include <cstdint>
#include <iostream>

#define SET_GET(T, name) \
private: \
	T m_##name; \
public: \
	T get##name() { return m_##name; }; \
	void set##name(const T name) { m_##name = name; };

class configManager
{
public:
	static configManager *getInstance() {
		if (instance == nullptr)
			instance = new configManager();
		return instance;
	};
        ~configManager();
	SET_GET(uint32_t, DataQueueSize);
	SET_GET(uint16_t, ListenPort);
	SET_GET(uint16_t, WorkersCount);
	SET_GET(uint16_t, DBPort);
	SET_GET(std::string, DBHost);
	SET_GET(std::string, DBUrl);
	SET_GET(uint16_t, CacheTime);
	SET_GET(uint16_t, ThreadSleepTime);
	SET_GET(uint32_t, DBWriteSize);
	SET_GET(boost::log::trivial::severity_level, LogLevel);
	SET_GET(bool, Dummy);
	SET_GET(bool, Debug);
	std::string getDBURL();

private:
	configManager();
	static configManager *instance;
};

#endif // CONFIGMANAGER_H
