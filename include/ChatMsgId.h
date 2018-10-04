#pragma once 

#include <cstdio>
#include <cstdlib>

typedef enum {
	NONE_MSG_ID = 0,

	rqCrteUser,						
	rsCrteUser,						

	rqLogin,
	rsLogin,

    rqMessage,
    rsMessage,

	END_MSG_ID
} MsgId;

