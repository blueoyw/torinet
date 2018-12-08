#include <hiredis/hiredis.h>

int main(int argc, char **argv) {
    redisReply *reply;
    // Connecting
    redisContext *c = redisConnect("127.0.0.1", 6379);

    if (c == NULL || c->err) {
        if (c) {
            printf("Connecting Error: %s\n", c->errstr);
        } else {
            printf("Can't allocate redis context\n");
        }   
        return -1; 
    }   
    // SET command
    reply = (redisReply*)redisCommand(c, "SET key Hello");
    if (reply == NULL) {
        printf("redisCommand reply is NULL: %s\n", c->errstr);
        return -1; 
    }   
    if (reply->type == REDIS_REPLY_ERROR) {
        printf("Command Error: %s\n", reply->str);
        freeReplyObject(reply);
        return -1; 
    }   
    printf("Command reply: %s\n", reply->str);
    freeReplyObject(reply);

    // GET command
    reply = (redisReply*)redisCommand(c, "GET key");
    if (reply == NULL) {
        printf("redisCommand reply is NULL\n");
        return -1; 
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        printf("Command Error: %s\n", reply->str);
        freeReplyObject(reply);
        return -1;
    }
    printf("Command reply: %s\n", reply->str);
    freeReplyObject(reply);

    // GET XXX
    reply = (redisReply*)redisCommand(c, "GET XXX");
    if (reply == NULL) {
        printf("redisCommand reply is NULL\n");
        return -1; 
    }   
    if (reply->type == REDIS_REPLY_ERROR) {
        printf("Command Error: %s\n", reply->str);
        freeReplyObject(reply);
        return -1; 
    }   
    printf("Command reply: %s\n", reply->str);
    freeReplyObject(reply);

    redisFree(c);
}
