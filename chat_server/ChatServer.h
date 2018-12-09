#pragma once

#include "Incl.h"
#include <map>

using namespace std;
using namespace tori;
using namespace tori::net;

class Redis;
class ChatServer {
public:
	ChatServer( const string name, const ServerConfig& config );

	void start(string ip, int port);
	void stop( const boost::system::error_code& error, int sigNum);

    //void openedHandler( const Ptr<TcpSession>& session);
    void openedHandler( const Ptr<Session>& session);
    void closedHandler( const Ptr<Session>& session, const CloseReason& reason);
    void messageHandler(const Ptr<Session>& session);

    Ptr<Character> getCharacter(int id );

private:
    //message handlers
    void rqCreateUser(const Ptr<Session>& session);
    void nfMessage(const Ptr<Session>& session);

private:

	Ptr<TcpServer> 				server_;
	boost::asio::io_service&	ios_;

    std::mutex lock_;
    map<int, Ptr<Room>>         rooms_;
    map<int, Ptr<Character>>    characters_;

    Ptr<Redis> redis_;
};
