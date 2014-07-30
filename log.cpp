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

#include <fstream>
#include <iomanip>
#include <boost/version.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#if (BOOST_VERSION) >= 105500
# include <boost/utility/empty_deleter.hpp>
#else
# include <boost/log/utility/empty_deleter.hpp>
#endif
#include <boost/log/expressions/formatters/named_scope.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <time.h>
#include "log.h"
#include "configmanager.h"

/*
void my_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
{
	time_t tt = time(NULL);
	char *z = ctime(&tt);
	z[strlen(z) - 1] = 0;
	strm << "[" << z << "]  ";
	strm << "[" << rec[boost::log::trivial::severity] << "]  : ";
//    strm << "[" << boost::log::expressions::attr< unsigned int >("LineID") << "]";
//    strm << "[" << boost::log::expressions::format_named_scope("Scopes", "%n") << "]  : ";
	strm << rec[boost::log::expressions::smessage];
}
*/

void initLog()
{
	boost::shared_ptr<boost::log::core> core = boost::log::core::get();

	configManager *cfg = configManager::getInstance();
	boost::log::trivial::severity_level logLevel = cfg->getLogLevel();
	core->set_filter(boost::log::trivial::severity >= logLevel);
//	core->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
//	core->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);

	return;
/*
	boost::shared_ptr< boost::log::sinks::text_ostream_backend > backend = boost::make_shared< boost::log::sinks::text_ostream_backend >();
#if (BOOST_VERSION) >= 105500
	backend->add_stream(boost::shared_ptr< std::ostream >(&std::clog, boost::empty_deleter()));
#else
	backend->add_stream(boost::shared_ptr< std::ostream >(&std::clog, boost::log::empty_deleter()));
#endif
	backend->auto_flush(true);

	typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > sink_t;
	boost::shared_ptr< sink_t > sink(new sink_t(backend));
	sink->set_formatter(&my_formatter);

	core->add_sink(sink);
*/
}
