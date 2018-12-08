#pragma once

#include "Incl.h"
#include <set>

using namespace tori;
using namespace tori::net;

class Redis
:public boost::enable_shared_from_this<Redis>
{
public:
	Redis( const char* host, const int32_t port)
        : host_(host), port_(port)
          ,context_(NULL), reply_(NULL)
	{
	}

	bool connect();
	string send(string command);

	string getHost(){ return host_; }
	int32_t	getPort(){ return port_; }
    redisContext* getRedisContext() { return context_; }
    redisReply* getRedisReply() { return reply_; }

private:
	string	host_;
	int32_t	port_;
    redisContext* context_;
    redisReply* reply_;
};
