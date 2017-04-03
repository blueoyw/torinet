#include "Client.h"
bool ClientHandler::read( uint32_t& msgId, int16_t& length, VoidPtr sess, VoidWeakPtr owner )
{
	TcpSessionPtr tmpSess = *(TcpSessionPtr*) (&sess);
	if( !tmpSess || owner.expired() )  {
		LOG(L_ERR, "[%s] msgId[%x] length[%d] is NULL session",__func__, msgId, length );
		return false;
	}

    /*
	LOG(L_INF, "[%s] msgId[%x] length[%d] use_count[%ld]",__func__, msgId, length
			, tmpSess.use_count() );
            */

	ClientWeakPtr weakClient = *(ClientWeakPtr*)&owner;
	ClientPtr client = weakClient.lock();

	if( !client) {
		LOG(L_ERR, "[%s] msgId[%x] length[%d] is NULL Client",__func__, msgId, length );
		return false;
	}

	switch (msgId) {
	case rs_CrteUser:
	{
	}
	break;
	case rs_Login:
	{
	}
	break;
	case rs_CrteParty:
	{
		rs_CrteParty_t*	msg = (rs_CrteParty_t*)(tmpSess->msg().body());
		client->m_partyId = msg->partyId;
	}
	break;
	case rs_JoinUser:
	{
		//rsJoinUser_t*	msg = (rsJoinUser_t*)(tmpSess->msg().body());
		//client->m_partyId = msg->partyId;
	}
	break;
	case rq_ReqJoin:
	{
		rq_ReqJoin_t*	msg = (rq_ReqJoin_t*)(tmpSess->msg().body());
		client->m_partyId = msg->partyId;
	}
	break;
	case nf_Message:
	{
		nf_Message_t*   msg = (nf_Message_t*)(tmpSess->msg().body());
		LOG(L_INF, "[%s] msg[%s] ",__func__, msg->msg );
	}
	break;
	case ID_nf_Eco:
	{
		//NfEco*   msg = (NfEco*)(tmpSess->msg().body());
        //LOG(L_INF, "[%s] msg[%s] ",__func__, msg->msg );
	}
	break;
	default :
		LOG(L_ERR, "[%s] unkown msgId[%x] ",__func__, msgId );
	break;
	}

	return true;
}

bool ClientHandler::connect( VoidPtr sess, VoidWeakPtr owner )
{
	if( owner.expired() ) {
		return false;
	}

	TcpSessionPtr tmpSess = *(TcpSessionPtr*)&sess;
	ClientWeakPtr weakClient = *(ClientWeakPtr*)&owner;
	ClientPtr	client = weakClient.lock();

	if( !client ) {
		return false;
	}

	tmpSess->asyncRead();
	
	return true;
}

extern bool g_bTerm;
bool ClientHandler::error( const boost::system::error_code& error, VoidPtr sess, VoidWeakPtr owner )
{
    LOG(L_FAT, "[%s] error:%s ", __func__, error.message().c_str() );
	if( owner.expired() ) {
		return false;
	}

	TcpSessionPtr tmpSess = *(TcpSessionPtr*)&sess;
	ClientWeakPtr weakClient = *(ClientWeakPtr*)&owner;
	ClientPtr	client = weakClient.lock();

	if( !client ) {
		return false;
	}

	//client->stop();
    g_bTerm = true;

	return true;
}

