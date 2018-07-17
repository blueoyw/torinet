#pragma once

#include "Incl.h"
#include <map>

/*
// Msg process
bool	createUser(  TcpSessionPtr& sess );
bool	login(  TcpSessionPtr& sess );
bool	createParty( TcpSessionPtr& sess );
bool	joinUser( TcpSessionPtr& sess );
bool	nfMessage( TcpSessionPtr& sess, int16_t& length);

// callback Msg Handler
class ServerHandler: public SessionHandler
{
public:
	ServerHandler(){}
	virtual ~ServerHandler(){}

	virtual bool   read( uint32_t& msgId, int16_t& length, VoidPtr sess, VoidWeakPtr owner );
	virtual bool   error( const boost::system::error_code& error, VoidPtr sess, VoidWeakPtr owner );
	virtual bool   connect( VoidPtr sess, VoidWeakPtr owner );
};
*/

using namespace std;
using namespace tori;
using namespace tori::net;

class WorldServer {
public:
	WorldServer( const string name, const ServerConfig& config );

	void start();
	void stop( const boost::system::error_code& error, int sigNum);

    void openedHandler( const Ptr<TcpSession>& session);
    void closedHandler( const Ptr<TcpSession>& session, const CloseReason& reason);
    void messageHandler( const Ptr<TcpSession>& session, const uint8_t* data, size_t size);
    void onMessage();

    Character getCharacter( const int id );

private:
	Ptr<TcpServer> 				m_server;
	boost::asio::io_service&	m_ios;

    std::mutex m_mutex;
    map<int, Ptr<Room>>         m_rooms;
    map<int, Ptr<Character>>    m_characters;
};
