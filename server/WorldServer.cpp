#include "WorldServer.h"

WorldServer::WorldServer()
		//: m_server(), m_ios(pool->getIoService()), m_signals(m_ios), m_pIoPool(pool)
{
    m_server.reset( new TcpServer( name, config) );
    m_ios = m_server->getIoServicePool()->getIoService();
    m_signals = m_ios;
    /*
private:
	boost::asio::io_service&	m_ios;
	boost::asio::signal_set		m_signals;

    std::mutex m_mutex;
	Ptr<TcpServer> 				m_server;
    map<int, Ptr<Room>>         m_rooms;
    map<int, Ptr<Character>>    m_characters;
    */
}

void WorldServer::regSignal()
{
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
    //m_signals.add(SIGSEGV);
    m_signals.async_wait( boost::bind(&WorldServer::stop, this,
                boost::asio::placeholders::error, 
                boost::asio::placeholders::signal_number ) ) ;
}


void WorldServer::start()
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

void WorldServer::stop( const boost::system::error_code& error, int sigNum)
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

void WorldServer::openedHandler( const Ptr<TcpSession>& session)
{

}

void WorldServer::closedHandler( const Ptr<TcpSession>& session, const CloseReason& reason)
{
}

void WorldServer::messageHandler( const Ptr<TcpSession>& session, const uint8_t* data, size_t size)
{

}

void WorldServer::onMessage()
{

}

Character WorldServer::getCharacter( const int id )
{

}

