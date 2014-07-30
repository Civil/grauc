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
 
#ifndef DBCLIENTDATA_H
#define DBCLIENTDATA_H

#include <iostream>

struct graphData {
	char name[256];
	double data;
	uint32_t timestamp;
};

#endif