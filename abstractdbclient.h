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

#ifndef ABSTRACTDBCLIENT_H
#define ABSTRACTDBCLIENT_H

#include <vector>
#include "dbclientdata.h"

class abstractDBClient
{
public:
	abstractDBClient();
	virtual ~abstractDBClient();

	virtual void insertData(std::vector<struct graphData *> & data, std::vector<struct graphData *> & cache) = 0;
};

#endif // ABSTRACTDBCLIENT_H
