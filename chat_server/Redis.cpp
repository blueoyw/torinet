#include "Redis.h"

bool Redis::connect(const string ip)
{
    host_ = ip;
    context_ = redisConnect(ip.c_str(), port_);

    if (context_ == NULL || context_->err)
    {
        if (context_)
            printf("Connecting Error: %s\n", context_->errstr);
        else
            printf("Can't allocate redis context\n");
        return false;
    }
    return true;
}

string Redis::send(string command)
{
    string rtn = "";
    if (context_ == NULL) 
    {
        printf("Error context is NULL\n");
        return rtn; 
    }   

    reply_ = (redisReply*)redisCommand(context_, command.c_str());
    if (reply_ == NULL) {
        printf("redisCommand reply is NULL: %s\n", context_->errstr);
        return rtn; 
    }   
    if (reply_->type == REDIS_REPLY_ERROR) {
        printf("Command Error: %s\n", reply_->str);
        freeReplyObject(reply_);
        return rtn; 
    }   
    printf("Command reply: %s\n", reply_->str);
    rtn = reply_->str;
    freeReplyObject(reply_);
    return rtn;
}
