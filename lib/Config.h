#pragma once
#include "Incl.h"

using namespace std;

namespace tori {
namespace net {

class ServerConfig
{
public:
	ServerConfig()
		: _threadCount(1), _maxSession(1024), _noDelay(true)
		  , _minReceive(1024*4), _maxReceiveBufferSize(numeric_limits<size_t>::max())
	{
	}

	void print() 
	{
		cout<< "threadCount=" << _threadCount ;
		cout<< "maxSession=" << _maxSession ;
		cout<< "noDelay=" << _noDelay;
		cout<< "_minReceive=" << _minReceive ;
		cout<< "_maxReceiveBufferSize=" << _maxReceiveBufferSize << endl;
	}

	size_t _threadCount;
	size_t _maxSession;
	bool   _noDelay;
	size_t _minReceive;
	size_t _maxReceiveBufferSize;
};

class ClientConfig
{
public:
	ClientConfig()
		: _noDelay(true)
		  , _minReceive(1024*4), _maxReceiveBufferSize(numeric_limits<size_t>::max())
	{
	}


	void print() 
	{
		cout<< "noDelay=" << _noDelay;
		cout<< "_minReceive=" << _minReceive ;
		cout<< "_maxReceiveBufferSize=" << _maxReceiveBufferSize << endl;
	}

	bool   _noDelay;
	size_t _minReceive;
	size_t _maxReceiveBufferSize;
};

}
}

