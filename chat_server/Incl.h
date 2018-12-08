#pragma once

#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>

#include <memory>
#include <thread>

#include <hiredis/hiredis.h>

#include "../include/com.h"
#include "../include/Msg.h"

#include "../Src/IoServicePool.h"
#include "../Src/TcpServer.h"
#include "../Src/TcpSession.h"

#include "Redis.h"
#include "GameObject.h"
#include "Character.h"
#include "Room.h"
//#include "WorldServer.h"
#include "ChatServer.h"
