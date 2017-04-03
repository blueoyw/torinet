#ifndef _INCL_H_
#define _INCL_H_

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <vector>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>

#include <sys/types.h>
#include <unistd.h>

#include "../include/com.h"
#include "../include/Msg.h"

#include "../lib/IoServicePool.h"
#include "../lib/TcpServer.h"
//#include "HandleMsg.h"
#include "Server.h"
#include "Character.h"
#include "Room.h"

typedef boost::unordered_map<string, CharacterPtr>				  CharactersMap;
typedef boost::unordered_map<string, CharacterPtr>::iterator  CharactersItor;
typedef boost::unordered_map<uint32_t, RoomPtr>				  	RoomMap;
typedef boost::unordered_map<uint32_t, RoomPtr>::iterator		RoomItor;

#endif
