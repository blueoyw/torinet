#ifndef _SERVER_H_
#define _SERVER_H_
#include "Incl.h"
#include "../lib/TcpSession.h"

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

typedef boost::shared_ptr<ServerHandler> ServerHandlerPtr;


extern pid_t ppid;

class Server {
public:
	Server(boost::asio::io_service& ios)
		:m_ios(ios), m_signals(ios), m_ioMode(SINGLE_ASIO)
	{

	}

	Server(IoServicePoolPtr pool)
		:m_ios(pool->getIoService()), m_signals(m_ios), m_pIoPool(pool), m_ioMode(MULTI_ASIO)
	{

	}

	void regSignal()
	{
		m_signals.add(SIGINT);
		m_signals.add(SIGTERM);
		//m_signals.add(SIGSEGV);
		m_signals.async_wait( boost::bind(&Server::stop, this,
					boost::asio::placeholders::error, 
					boost::asio::placeholders::signal_number ) ) ;
	}

	void start(const int& numThreads)
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
			LOG(L_INF, "Open Server 50000");

			m_pIoPool->run();
		} else {
			m_server.reset(new TcpServer(m_ios, endpoint, numThreads, handler));
			m_server->startAccept();
			m_server->startTmr();
			LOG(L_INF, "Open Server 50000");

			std::vector< boost::shared_ptr<boost::thread> > threads;
			for ( int i=0; i< numThreads; i++ ) {
				boost::shared_ptr<boost::thread> thread(new boost::thread(
							boost::bind(&boost::asio::io_service::run, &m_ios)) );
				threads.push_back(thread);
			}

			m_ios.run();

			for ( size_t i=0; i< threads.size() ; i++ ) {
				threads[i]->join();
			}
		}
	}

	void stop( const boost::system::error_code& error, int sigNum)
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

private:
	boost::asio::io_service&	m_ios;
	boost::asio::signal_set		m_signals;
	TcpServerPtr 					m_server;
	IoServicePoolPtr				m_pIoPool;
	IoMode							m_ioMode;
};

typedef boost::shared_ptr<Server> ServerPtr;

#endif
