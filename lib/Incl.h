#pragma once

#include <algorithm>
#include <stdlib.h>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <vector>

#include <memory>
/*
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>
*/

#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>

/*
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>
*/

#include <sys/types.h>
#include <unistd.h>

//#include "../include/com.h"
#include "../include/Msg.h"

enum IoMode { SINGLE, MULTI };

using VoidPtr = std::shared_ptr<void>;
using VoidWeakPtr = std::weak_ptr<void>;

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

using boost::system::error_code;
namespace asio = boost::asio;
