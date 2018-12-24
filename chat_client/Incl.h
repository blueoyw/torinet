#pragma once

/*
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>

#include <memory>
#include <thread>

#include "../include/com.h"
#include "../include/Msg.h"

#include "../Src/IoServicePool.h"
#include "../Src/TcpSession.h"
*/

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
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>

#include <sys/types.h>
#include <unistd.h>

#include "../include/com.h"
#include "../include/Msg.h"

#include "../Src/TcpSession.h"
#include "../Src/IoServicePool.h"

#include "Client.h"
#include "Cli.h"
