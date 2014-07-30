cmake_minimum_required(VERSION 2.8)

project(grauc)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -std=gnu++1y -DBOOST_ALL_DYN_LINK -O3 -march=native -mtune=native -flto -ftracer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -std=gnu++1y -DBOOST_ALL_DYN_LINK -g -ggdb")

find_package( Boost 1.54 COMPONENTS system atomic log log_setup thread REQUIRED)
find_package( CURL REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#add_definitions(-pg -g)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

add_executable(grauc influxdbwriter.cpp abstractdbclient.cpp log.cpp configmanager.cpp worker.cpp clickhousewriter.cpp listner.cpp  main.cpp)
target_link_libraries( grauc ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY}
	${Boost_LOG_SETUP_LIBRARY} ${Boost_THREAD_LIBRARY} ${CURL_LIBRARY}
	-ltcmalloc -lev )

install(TARGETS grauc RUNTIME DESTINATION bin)
