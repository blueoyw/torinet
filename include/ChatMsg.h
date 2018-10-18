#pragma once 

#include "Incl.h"
#include "ChatMsgId.h"
#include <deque>

namespace tori
{
namespace message
{

const int MAX_MSG_LENGTH = 1024;

struct MessageHeader {
	int8_t		version;			
	int8_t		result;			
	int16_t		bodyLength;
	uint32_t	msgId;
};

struct MessageBody {
	unsigned char  data[MAX_MSG_LENGTH];
};

struct Message{
	MessageHeader	hdr;
	MessageBody		body;
} __attribute__((packed));

/////////////////////////////////////////////////////
// util class
/////////////////////////////////////////////////////

//POD type class
class MessageManager
{
public:
    enum { hdrLength = sizeof(cpMsgHeader_t) };
    enum { msgIdLength = sizeof(int) };
    enum { version = 1 };
    //enum { max_body_length = MAX_CLI_DATA-sizeof(cliHeader_t) };

    MessageManager()
    {
    }

    MessageManager(Message& msg):msg_(msg)
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

    const Message* data() const
    {
        return &msg_;
    }

    Message* data()
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
    Message     msg_;
};

typedef std::deque<MessageManager> MessageQueue;

/////////////////////////////////////////////////////
// body structures
/////////////////////////////////////////////////////
//////////// MSG BODY////////////////////
struct RqCreateUser
{
    RqCreateUser()
    {
        memset(id, 0x00, sizeof(id));
    }
	char id[36];		// 1: success  0: fail 				
};

struct RsCreateUser
{
    enum ErrorCode
    {
        ERROR_NONE = 0,
        ERROR_ALREADY_USE
    };

    RsCreateUser()
        :errorCode(ERROR_NONE)
    {
        memset(id, 0x00, sizeof(id));
    }

    ErrorCode errorCode;
	char id[36];		// 1: success  0: fail 				
};

struct RqMessage
{
    RqMessage()
        :length(0)
    {
        memset(message, 0x00, sizeof(id));
    }

    uint32_t length;
    char message[128];
};

struct RsMessage
{
    enum ErrorCode
    {
        ERROR_NONE = 0,
        ERROR_SEND
    };

    RsMessage()
        :errorCode(ERROR_NONE)
    {
    }

    ErrorCode errorCode;
};

}
}
