#pragma once 

#include <cstdio>
#include <cstdlib>

typedef enum {
	NONE_MSG_ID = 0,

	rqCreateUser,						
	rsCreateUser,						

    rqMessage,
    rsMessage,

	END_MSG_ID
} MsgId;

