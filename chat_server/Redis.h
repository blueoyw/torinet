#pragma once

#include "Incl.h"
#include <set>

using namespace tori;
using namespace tori::net;

class Redis
:public boost::enable_shared_from_this<Redis>
{
public:
    enum { REDIS_PORT = 6379 };
	Redis()
        : host_(""), port_(REDIS_PORT)
          ,context_(NULL), reply_(NULL)
	{
	}

    ~Redis()
    {
        if (context_ != NULL)
            redisFree(context_);
    }

	bool connect(const string host);
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
