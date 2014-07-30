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

#ifndef INFLUXDBWRITER_H
#define INFLUXDBWRITER_H

#include <iostream>
#include "abstractdbclient.h"
#include <curl/curl.h>

class influxdbWriter : abstractDBClient
{
public:
	influxdbWriter();
	virtual ~influxdbWriter();

	virtual void insertData(std::vector<struct graphData *> & data, std::vector<struct graphData *> & cache);
private:
	CURL *m_curl;
	std::string m_tmpString;
};


#endif // INFLUXDBWRITER_H
