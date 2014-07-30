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

#ifndef LOG_H
#define LOG_H

#include <iomanip>
#include <boost/log/trivial.hpp>

#define VERB_FMT " ["<<std::setw(24)<<__FUNCTION__<<"]  : "
#define TRACE BOOST_LOG_TRIVIAL(trace)<<"      "<<VERB_FMT
#define DEBUG BOOST_LOG_TRIVIAL(debug)<<"      "<<VERB_FMT
#define INFO  BOOST_LOG_TRIVIAL(info)<<"      "<<VERB_FMT
#define WARN  BOOST_LOG_TRIVIAL(warning)<<"      "<<VERB_FMT
#define ERROR BOOST_LOG_TRIVIAL(error)<<"      "<<VERB_FMT
#define FATAL BOOST_LOG_TRIVIAL(fatal)<<"      "<<VERB_FMT

void initLog();

#endif // LOG_H
