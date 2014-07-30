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
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include "configmanager.h"
#include "influxdbwriter.h"
#include "log.h"
#include "common.h"

influxdbWriter::influxdbWriter()
{
	m_curl = curl_easy_init();
	if (!m_curl) {
		throw("Failed to initialize curl");
	}
	curl_easy_setopt(m_curl, CURLOPT_URL, configManager::getInstance()->getDBURL().c_str());
	curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1);

	m_tmpString.reserve(8*1024*1024);
	
	DEBUG<<"InfluxDB Writer initialized";
}

influxdbWriter::~influxdbWriter()
{
	curl_easy_cleanup(m_curl);
}

void influxdbWriter::insertData (std::vector<struct graphData *> & dataVec, std::vector<struct graphData *> & cache)
{
	CURLcode res;
	struct graphData *data = nullptr;
	bool dummy = configManager::getInstance()->getDummy();
	bool debug = configManager::getInstance()->getDebug();
	std::unordered_map<char *, std::vector<struct graphData *>> metrics;
	std::vector<char *> metricNames;
	uint32_t i;
	uint32_t size = dataVec.size();

	if (debug) {
		DEBUG<<"Insert called";
	}

	m_tmpString = "[";

	for (i = 0; i < size; i++) {
		data = dataVec.back();
		dataVec.pop_back();
		if (data->name == 0) {
			cache.push_back(data);
			continue;
		}

		m_tmpString += std::string(" { \"name\": \"")
			       + std::string(data->name)
			       + std::string("\", \n")
			       + std::string("    \"columns\": [\"time\", \"value\"],\n")
			       + std::string("    \"points\": [\n")
			       + std::string("      [")
			       + boost::lexical_cast<std::string>(data->timestamp)
			       + std::string(", ")
			       + boost::lexical_cast<std::string>(data->data)
			       + std::string("]\n    ]\n  }");
		if ( (size - 1) != i) {
			m_tmpString += ",";
		}
		m_tmpString += "\n";
		cache.push_back(data);
	}
	m_tmpString += "]";


	if (debug) {	
		INFO<<"i='"<<i<<"' size='"<<size<<" query: "<<std::endl<<m_tmpString;
	}
	if (!dummy) {
		curl_easy_setopt(m_curl, CURLOPT_POST, 1);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_tmpString.c_str());
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, m_tmpString.length()); 
		res = curl_easy_perform(m_curl);
		if (res != CURLE_OK) {
			ERROR<<"curl_perform failed: "<<curl_easy_strerror(res);
		}

//		TRACE<<curl_easy_strerror(res);
	} else {
		DEBUG<<"Dummy mode enabled, dropping data";
	}

	return;
}

