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

bool Redis::send(string command)
{
    LOG(L_INF, "[%s] command[%s]", __func__, command.c_str());
    bool rtn = false;
    if (context_ == NULL) 
    {
        LOG(L_ERR, "[%s] context is NULL", __func__);
        return rtn; 
    }   

    reply_ = (redisReply*)redisCommand(context_, command.c_str());
    if (reply_ == NULL) {
        LOG(L_ERR, "[%s] reply is NULL. %s", __func__, context_->errstr);
        return rtn; 
    }   

    if (reply_->type == REDIS_REPLY_ERROR) {
        LOG(L_ERR, "[%s] Command Error. %s", __func__, reply_->str);
        printf("Command Error: %s\n", reply_->str);
    }   
    else if (reply_->type == REDIS_REPLY_STRING) 
    {
        rtn = true;
        LOG(L_DEB, "[%s] REDIS_REPLY_STRING reply_->type[%d] reply_->str[%s]."
            , __func__
            ,reply_->type, reply_->str);

    }
    else if (reply_->type == REDIS_REPLY_INTEGER) 
    {
        rtn = true;
        LOG(L_DEB, "[%s] REDIS_REPLY_INTEGER reply_->type[%d] reply_->integer[%d]."
            , __func__
            ,reply_->type, reply_->integer);

    }
    else if (reply_->type == REDIS_REPLY_ARRAY) 
    {
        rtn = true;
        for (int i=0; i<reply_->elements; i++)
        {
            LOG(L_DEB, "[%s]REDIS_REPLY_ARRAY[%d] reply_->type[%d] reply_->elements[%s]."
                , __func__, i
                ,reply_->type, reply_->element[i]->str);
        }
    }

    freeReplyObject(reply_);
    return rtn;
}
