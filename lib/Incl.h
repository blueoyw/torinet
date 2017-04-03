#ifndef _NETWORK_INCL_H_
#define _NETWORK_INCL_H_

#include <algorithm>
#include <stdlib.h>
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

typedef boost::shared_ptr<void>	VoidPtr;
typedef boost::weak_ptr<void> 	VoidWeakPtr;

typedef enum {
	SINGLE_ASIO=0,
	MULTI_ASIO
} IoMode;

#endif
