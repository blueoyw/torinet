// 2013.08.12
//	version : comment
//	   1    : first commit

#ifndef CP_MSG_H_
#define CP_MSG_H_

#include "Incl.h"
#include "MsgId.h"
#include <deque>

using namespace std;
const int SERVER_TCP_PORT 	  = 40000;
const int CLIENT_TCP_PORT 	  = 40001;

const int MAX_CP_MSG_DATA 	  = 1024;

typedef struct {
	int8_t		version;			
	int8_t		result;			
	int16_t		bodyLength;
	uint32_t		msgId;
} cpMsgHeader_t;

typedef struct {
	unsigned char  data[MAX_CP_MSG_DATA];
} cpMsgBody_t;

typedef struct {
	cpMsgHeader_t	hdr;
	cpMsgBody_t		body;
} __attribute__((packed)) cpMsg_t;

/////////////////////////////////////////////////////
// util class
/////////////////////////////////////////////////////

//POD type class
class Msg
{
public:
    enum { hdrLength = sizeof(cpMsgHeader_t) };
    enum { msgIdLength = sizeof(int) };
    enum { version = 1 };
    //enum { max_body_length = MAX_CLI_DATA-sizeof(cliHeader_t) };

    Msg()
    {
    }

    Msg(cpMsg_t& msg):msg_(msg)
    {
    }

    const cpMsgHeader_t* head() const
    {
        return &msg_.hdr;
    }

    cpMsgHeader_t* head()
    {
        return &msg_.hdr;
    }

    const cpMsg_t* data() const
    {
        return &msg_;
    }

    cpMsg_t* data()
    {
        return &msg_;
    }

    int length() const
    {
        return hdrLength + msg_.hdr.bodyLength;
    }

    cpMsgBody_t* body()
    {
        return &msg_.body;
    }

    int bodyLength() const
    {
        return msg_.hdr.bodyLength;
    }

    int msgId() const
    {
        return msg_.hdr.msgId;
    }

    void setBodyLength(int length)
    {
        msg_.hdr.bodyLength = length;
        if (msg_.hdr.bodyLength > MAX_CP_MSG_DATA)
            msg_.hdr.bodyLength = MAX_CP_MSG_DATA;
    }

    bool checkHeader()
    {
        if ( msg_.hdr.version != version ) {
            //cout << "version is not match" << msg_.hdr.version << "\n";
            return false;
        }

        if (  msg_.hdr.bodyLength > MAX_CP_MSG_DATA ) {
            //cout << "bodyLength is too big:" << msg_.hdr.bodyLength << "\n";
            return false;
        }

        if (  msg_.hdr.result > ERROR ) {
            //cout << "bodyLength is too big:" << msg_.hdr.bodyLength << "\n";
            return false;
        }

        return true;
    }

    void encodeMsg(const MsgId_t& msgId, const ResponseCode_t& result, const unsigned char* data, const int& dataLength)
    {
        msg_.hdr.version = version;
        msg_.hdr.msgId = msgId;
        msg_.hdr.result = result;

        msg_.hdr.bodyLength = dataLength;

        if( msg_.hdr.bodyLength>0 && data) {
            memcpy( msg_.body.data, data, dataLength);
        }

    }

    void encodeMsg(const MsgId_t& msgId, const unsigned char* data, const int& dataLength)
    {
        msg_.hdr.version = version;
        msg_.hdr.msgId = msgId;
        msg_.hdr.result = NONE_CODE;

        msg_.hdr.bodyLength = dataLength;

        if( msg_.hdr.bodyLength>0 && data) {
            memcpy( msg_.body.data, data, dataLength);
        }

    }

private:
    cpMsg_t     msg_;
};

typedef std::deque<Msg> MessageQueue;

/////////////////////////////////////////////////////
// body structures
/////////////////////////////////////////////////////
//////////// MSG BODY////////////////////
typedef struct {
	char				id[36];		// 1: success  0: fail 				
	char				passwd[36];		// fail reason
} rq_CrteUser_t;

typedef rq_CrteUser_t rq_Login_t;

typedef struct {
	char				id[36];		// 1: success  0: fail 				
} rq_CrteParty_t;

typedef struct {
	uint32_t			partyId;			// result code
} rs_CrteParty_t;

typedef struct {
	uint16_t			userCnt;
	char				joinUserId[36];		//id list sperated by ";"
} partyList_t;

typedef struct {
	char				fromId[36];		// from 				
	char				toId[36];		// to				
	uint32_t			partyId;		// 1: success  0: fail 				
} rq_JoinUser_t;

typedef rs_CrteParty_t rs_JoinUser_t;
typedef rq_JoinUser_t rq_ReqJoin_t;

typedef struct {
	uint32_t			partyId;
	char				msg[512];		// from 				
} nf_Message_t;

struct NfEco
{
	char				msg[512];		// from 				
    NfEco()
    {
        memset( msg, 0x00, sizeof(msg));
    }
};

#endif // CP_MSG_H_
