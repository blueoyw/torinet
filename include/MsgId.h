#ifndef CP_MSG_ID
#define CP_MSG_ID

#include <cstdio>
#include <cstdlib>

typedef enum {
	NONE_MSG_ID = 0,

	CHARACTER_MSG_ID	= 0x1000,

	rq_CrteUser,						
	rs_CrteUser,						
	rq_Login,
	rs_Login,

	rq_CrteParty,					
	rs_CrteParty,				

	rq_JoinUser,					
	rs_JoinUser,					

	rq_ReqJoin,					
	rs_ReqJoin,					

	nf_Message,					

	ID_nf_Eco,					

	END_MSG_ID = 0xFFFF	
} MsgId_t;

typedef enum {
	NONE_CODE = 0x0000,

	CHARACTER = 0x1000,
	CREATE_USER_SUCCESS,
	LOGIN_SUCCESS,
	CREATE_PARTY_SUCCESS,
	JOIN_USER_SUCCESS,
	NOTIFY_MESSAGE,

	ERROR = 0x2000,
	CREATE_USER_FAIL,
	LOGIN_FAIL,
	CREATE_PARTY_FAIL,
	JOIN_USER_FAIL,

	INVALID_MSG,

	END_RES_CODE = 0xFFFF

} ResponseCode_t;


#endif // CP_MSG_ID
