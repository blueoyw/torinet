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

class WorldServer {
public:
	WorldServer()
		//: m_server(), m_ios(pool->getIoService()), m_signals(m_ios), m_pIoPool(pool)
	{
        m_server.reset( new TcpServer( name, config) );
        m_ios = m_server->getIoServicePool()->getIoService();
        m_signals = m_ios;
	}

	void regSignal()
	{
		m_signals.add(SIGINT);
		m_signals.add(SIGTERM);
		//m_signals.add(SIGSEGV);
		m_signals.async_wait( boost::bind(&WorldServer::stop, this,
					boost::asio::placeholders::error, 
					boost::asio::placeholders::signal_number ) ) ;
	}


	void start();
    /*
	{
		regSignal(); 

		// cliServer port 50000 for cliClient
		tcp::endpoint endpoint(tcp::v4(), 50000);

		// Message Handler
		ServerHandlerPtr	handler(new ServerHandler());

		if( m_ioMode == MULTI_ASIO ) {
			m_server.reset(new TcpServer(endpoint, numThreads, handler, m_pIoPool));
			m_server->startAccept();
			m_server->startTmr();
			LOG(L_INF, "Open WorldServer 50000");

			m_pIoPool->run();
		}
	}
    */

	void stop( const boost::system::error_code& error, int sigNum);
    /*
	{
		LOG(L_FAT, "[%s] sigNum[%d]", __func__, sigNum);
		if(error) {
			LOG(L_FAT, "[%s] error[%s]", __func__, error.message().c_str() );
			return;
		}

		m_server->handleStop();
		if( m_ioMode == MULTI_ASIO ) {
			m_pIoPool->stop();
			return;
		}
		m_ios.stop();
	}
    */

    void openedHandler( const Ptr<TcpSession>& session);
    void closedHandler( const Ptr<TcpSession>& session, const CloseReason& reason);
    void messageHandler( const Ptr<TcpSession>& session, const uint8_t* data, size_t size);
    void onMessage();

    Character getCharacter( const int id );

private:
	boost::asio::io_service&	m_ios;
	boost::asio::signal_set		m_signals;

    std::mutex m_mutex;
	Ptr<TcpServer> 				m_server;
    map<int, Ptr<Room>>         m_rooms;
    map<int, Ptr<Character>>    m_characters;
};
