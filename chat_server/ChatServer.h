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

class ChatServer {
public:
	ChatServer( const string name, const ServerConfig& config );

	void start(string ip, int port);
	void stop( const boost::system::error_code& error, int sigNum);

    //void openedHandler( const Ptr<TcpSession>& session);
    void openedHandler( const Ptr<Session>& session);
    void closedHandler( const Ptr<Session>& session, const CloseReason& reason);
    void messageHandler( const Ptr<Session>& session, const uint8_t* data, size_t size);
    void onMessage();

    Ptr<Character> getCharacter(int id );

private:
	Ptr<TcpServer> 				server_;
	boost::asio::io_service&	ios_;

    std::mutex lock_;
    map<int, Ptr<Room>>         rooms_;
    map<int, Ptr<Character>>    characters_;
};
