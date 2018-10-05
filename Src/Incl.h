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
#include <thread>
#include <memory>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>

#include <sys/types.h>
#include <unistd.h>

#include "../include/com.h"
#include "../include/Msg.h"

#include "Config.h"

namespace tori {
	template<typename T>
	using Ptr = std::shared_ptr<T>;

	template<typename T>
	using WeakPtr = std::weak_ptr<T>;

	template<typename T>
	using UniquePtr = std::unique_ptr<T>;

	namespace asio = boost::asio;
	using boost::system::error_code;

	typedef boost::shared_ptr<void>	VoidPtr;
	typedef boost::weak_ptr<void> 	VoidWeakPtr;

	typedef enum {
		SINGLE_ASIO=0,
		MULTI_ASIO
	} IoMode;

}
