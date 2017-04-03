#include "Server.h"
extern ServerPtr server;

extern CharactersMap  g_characters;
extern boost::mutex      g_charactersLock;

extern int32_t			g_roomId;
extern RoomMap    	g_rooms;
extern boost::mutex  g_roomsLock;

bool createUser( TcpSessionPtr& sess )
{
	//rq_CrteUser_t* msg = (rq_CrteUser_t*)(sess->msg().body());
	Msg msg;
	msg.encodeMsg( rs_CrteUser, CREATE_USER_SUCCESS, NULL, 0);
	sess->asyncSend( msg );
	return true;
}

bool login( TcpSessionPtr& sess )
{
	rq_Login_t* msg = (rq_Login_t*)(sess->msg().body());
	Msg sendMsg;

	string id( msg->id );
	string name( msg->id );
	CharacterPtr  ch(new Character( id, name, sess ) );
	g_charactersLock.lock();
	g_characters.insert( std::make_pair(id, ch) );
	g_charactersLock.unlock();
	sess->setId(id);

	sendMsg.encodeMsg( rs_Login, LOGIN_SUCCESS, NULL, 0);
	sess->asyncSend( sendMsg );
	return true;
}

bool createParty( TcpSessionPtr& sess )
{
	rq_CrteParty_t* msg = (rq_CrteParty_t*)(sess->msg().body());
	Msg sendMsg;

	// create character in the world
	g_charactersLock.lock();
	string id( msg->id );
	CharactersItor itor = g_characters.find( id );
	g_charactersLock.unlock();
	LOG(L_DEB, "[%s] id[%s] ", __func__, msg->id);
	if ( itor == g_characters.end() ) {
		sendMsg.encodeMsg( rs_CrteParty, CREATE_PARTY_FAIL, NULL, 0);
	    sess->asyncSend( sendMsg );
		LOG(L_ERR, "[%s] id[%s] character not found", __func__, msg->id);
		return false;
	}

	CharacterPtr ch = g_characters[id];

	rs_CrteParty_t rsCrteParty;
	RoomPtr room(new Room( ch ) );
	g_roomsLock.lock();
	bool empty = false;
	if ( g_rooms.find( g_roomId ) == g_rooms.end() ) {
		room->setId( g_roomId );
		room->setCharacter( ch );
		ch->setRoom( room );
		//g_rooms.insert( std::make_pair(g_roomId, room));
		g_rooms[ g_roomId ] = room;
		rsCrteParty.partyId = g_roomId++;
		empty = true;
	}
	g_roomsLock.unlock();
	
	if( !empty ) {
		sendMsg.encodeMsg( rs_CrteParty, CREATE_PARTY_FAIL, NULL, 0);
	    sess->asyncSend( sendMsg );
		LOG(L_ERR, "[%s] room_id[%d] not found",__func__, g_roomId-1);
		return false;
	}

	LOG(L_DEB, "[%s] send rs_CrteParty partyId[%d] room[%p] use_count[%ld] ", __func__, 
		rsCrteParty.partyId, room.get(), room.use_count() );
	sendMsg.encodeMsg( rs_CrteParty, CREATE_PARTY_SUCCESS, (uint8_t*)&rsCrteParty, sizeof(rsCrteParty));
    sess->asyncSend( sendMsg );
	return true;
}

bool  joinUser( TcpSessionPtr& sess )
{
	rq_JoinUser_t* msg = (rq_JoinUser_t*)(sess->msg().body());
	Msg sendMsg;
	LOG(L_DEB, "[%s] from_id[%s] to_id[%s] partyId[%d]",__func__, msg->fromId, 
		msg->toId, msg->partyId);

	bool rtn = true;
	CharacterPtr ch;
	g_charactersLock.lock();
	if ( g_characters.find( msg->toId ) == g_characters.end() ) {
		rtn = false;
	} else 
		ch = g_characters[msg->toId];
	g_charactersLock.unlock();

	if( !rtn || !ch || !ch->session() ) {
		sendMsg.encodeMsg( rs_JoinUser, JOIN_USER_FAIL, NULL, 0);
	    sess->asyncSend( sendMsg );
		LOG(L_ERR, "[%s] id[%s] character not found", __func__, msg->toId);
		return false;
	}

	g_roomsLock.lock();
	RoomPtr room;
	if ( g_rooms.find( msg->partyId ) == g_rooms.end() ) {
		rtn = false;
		LOG(L_ERR, "[%s] id[%d] room not found", __func__, msg->partyId);
	} else {
		room = g_rooms[msg->partyId];
		if( !room->setCharacter( ch ) )
			rtn = false;
		else
			ch->setRoom( room );

		LOG(L_DEB, "[%s] partyId[%d] room[%p] participants[%d]", __func__, 
				msg->partyId, room.get(), room->cnt());
	}
	g_roomsLock.unlock();
	
	if( !rtn ) {
		sendMsg.encodeMsg( rs_JoinUser, JOIN_USER_FAIL, NULL, 0);
	    sess->asyncSend( sendMsg );
		return false;
	}

	sendMsg.encodeMsg( rs_JoinUser, JOIN_USER_SUCCESS, NULL, 0);
    sess->asyncSend( sendMsg );

	rq_ReqJoin_t req_join_msg;
	req_join_msg.partyId = msg->partyId;
	strcpy(req_join_msg.fromId, msg->fromId);
	strcpy(req_join_msg.toId, msg->toId);

	sendMsg.encodeMsg( rq_ReqJoin, (uint8_t*)&req_join_msg, sizeof(rq_ReqJoin_t));
	//ch->session()->encode_sendMsg( rq_ReqJoin, (uint8_t*)&req_join_msg, sizeof(req_join_msg));
	ch->session()->asyncSend( sendMsg );
	return true;
}

bool  nfMessage( TcpSessionPtr& sess, int16_t& len )
{
	nf_Message_t* msg = (nf_Message_t*)(sess->msg().body());

	g_roomsLock.lock();
	bool empty = true;

	if ( g_rooms.find( msg->partyId ) != g_rooms.end() ) {
		RoomPtr room = g_rooms[msg->partyId];
		CharacterPtr ch;
		LOG(L_DEB, "[%s] partyId[%d] room[%p] participants[%d]", __func__, 
			msg->partyId, room.get(), room->cnt());
		for(int i=0; i< room->cnt(); i++) {
			if ( !room->getCharacter(i, ch ) )
				continue;
			Msg sendMsg ;
			nf_Message_t	nfMessage;
			strcpy(nfMessage.msg, msg->msg);
			sendMsg.encodeMsg( nf_Message, (uint8_t*)&nfMessage, sizeof(nf_Message_t));
	        ch->session()->asyncSend( sendMsg );
			LOG(L_DEB, "[%s] sess[%p] send msgId[%X] msg[%s]", __func__, ch->session().get(),
					nf_Message, nfMessage.msg);
		}

		empty = false;
	}
	g_roomsLock.unlock();
	
	if( empty ) {
		LOG(L_ERR, "[%s] party id[%d] not found", __func__, msg->partyId);
		return false;
	}

	return true;

}

bool  nfEco( TcpSessionPtr& sess, int16_t& len,  TcpServerPtr& svr )
{
	NfEco* msg = (NfEco*)(sess->msg().body());

	Msg sendMsg;
    NfEco	nfEco;
    memcpy( nfEco.msg, msg->msg, len);
    sendMsg.encodeMsg( ID_nf_Eco, (uint8_t*)&nfEco, len);
	sess->asyncSend( sendMsg );
    svr->addTx( sendMsg.length() );
	return true;
}

bool ServerHandler::read( uint32_t& msgId, int16_t& length, VoidPtr sess, VoidWeakPtr owner )
{
	TcpSessionPtr tmpSess = *(TcpSessionPtr*) (&sess);
    /*
	LOG(L_DEB, "[%s] msgId[%x] length[%d] use_count[%ld]",__func__, msgId, length
			, tmpSess.use_count() );
            */

	if( owner.expired() )
    {
        LOG(L_FAT, "[%s] owner expired",lfunc );
        return false;
	}

    TcpServerWeakPtr tmp = *(TcpServerWeakPtr*)&owner;
    TcpServerPtr server = tmp.lock();
    if( !server ) 
    {
        LOG(L_FAT, "[%s] server is NULL",lfunc );
        return false;
	}
    server->addRx( tmpSess->msg().length() );

	bool	rtn = false;
	
	switch(msgId)
	{
	case rq_CrteUser:
		rtn = createUser( tmpSess );
		break;
	case rq_Login:
		rtn = login( tmpSess );
		break;
	case rq_CrteParty:
		rtn = createParty( tmpSess );
		break;
	case rq_JoinUser:
		rtn = joinUser( tmpSess );
		break;
#if 0
	case rsReqJoin:
		rtn = req_join();
		break;
#endif
	case nf_Message:
		rtn = nfMessage( tmpSess, length );
		break;
	case ID_nf_Eco:
		rtn = nfEco( tmpSess, length, server );
		break;
	default:
		rtn = false;
		LOG(L_ERR, "[%s] unkwon message", __func__);
		break;
	}


	return rtn;
}

bool ServerHandler::error( const boost::system::error_code& error, VoidPtr sess, VoidWeakPtr owner )
{
    LOG(L_FAT, "[%s] error:%s ", __func__, error.message().c_str() );

	if( !error ) {
		return true;
	}

	if( owner.expired() ) {
		return false;
	}

	TcpServerWeakPtr tmp = *(TcpServerWeakPtr*)&owner;
	TcpServerPtr server = tmp.lock();
	TcpSessionPtr tmpSess = *(TcpSessionPtr*)&sess;

	//ywoh to do
	//cannot find character and room from session
	if( !tmpSess->id().empty() ) 
    {
		LOG(L_ERR, "[%s] session id[%s]", __func__, tmpSess->id().c_str() );
		CharacterPtr ch;
		g_charactersLock.lock();
		if( g_characters.find(tmpSess->id()) != g_characters.end() ) {
			ch = g_characters[tmpSess->id()];
		}
		g_charactersLock.unlock();

		if( ch ) {
			LOG(L_ERR, "[%s] session id[%s] character id[%s]", __func__, tmpSess->id().c_str(), ch->id().c_str() );
			RoomWeakPtr weakRoom = ch->m_room;
			RoomPtr room = weakRoom.lock();
			if( room ) {
				room->leave(ch);
				if ( !room->cnt() ) {
					g_roomsLock.lock();
					g_rooms.erase(room->id());
					g_roomsLock.unlock();
				}
			}

			g_charactersLock.lock();
			g_characters.erase(ch->id());
			g_charactersLock.unlock();
		}
	}

	if( server ) 
		server->deleteSession(tmpSess);

	return true;
}

bool   ServerHandler::connect( VoidPtr sess, VoidWeakPtr owner )
{
	return true;
}
