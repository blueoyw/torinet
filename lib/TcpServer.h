#pragma once
#include "Incl.h"
//#include "IoServicePool.h"
//#include "ISession.h"
#include "IServer.h"
#include "TcpSession.h"
#include <mutex>
#include <map>
#include <deque>

class TcpServer
:public IServer, public std::enable_shared_from_this<TcpServer>
{
public:
	using tcp = boost::asio::ip::tcp;

	TcpServer( size_t numThreads, IoMode mode );

	TcpServer(const TcpServer&) = delete;
	TcpServer& operator=(const TcpServer&) = delete;

	virtual void start(uint16_t port) override ;
	virtual void start(std::string ip, uint16_t port) override ;
	virtual void stop() override ;

	virtual State getState() override { return _state; }
	virtual Ptr<IoServicePool> getIoServicePool() override {return _iosPool; }

	virtual Ptr<ISession> getSession( long id ) override ;
	
	virtual void registerConnectHandler( ConnectHandler handler )
	{
		_connectHandler = handler;
	}

	virtual void registerReadHandler( ReadHandler handler )
	{
		_readHandler = handler;
	}

	virtual void registerErrorHandler( ErrorHandler handler )
	{
		_errorHandler = handler;
	}

	void listen( tcp::endpoint endpoint );
	void startAccept();

	/*
	boost::shared_ptr< TcpServer > thisObj(){ return shared_from_this(); }

	void handleStop()
	{
		LOG(L_FAT, "[%s]", __func__);
		m_sessions.clear();
	}

	void startAccept()
	{
		LOG(L_INF, "[%s] mode[%d]", __func__, m_ioMode);
		TcpSessionPtr session;
		if ( m_ioMode == MULTI ) {
            //load balancing rule : round robin
			session.reset( new TcpSession( m_pIosPool->getIoService(), m_ioMode, m_handler ));
		} else {
			session.reset( new TcpSession( m_ios, m_ioMode, m_handler ));
		}

		m_acceptor.async_accept(session->socket(),
				boost::bind(&TcpServer::handleAccept, shared_from_this(), session,
					boost::asio::placeholders::error));
	}

	void handleAccept(TcpSessionPtr session,
			const boost::system::error_code& error)
	{
		string tid = boost::lexical_cast<std::string>(boost::this_thread::get_id());
		LOG(L_INF, "[%s] thread id[%s]", __func__, tid.c_str());
		if (!error) 
        {
			m_sessions.insert(session);
			LOG(L_INF, "[%s] pushed size[%ld]", __func__, m_sessions.size());
			//session->set_server( m_sessions.size(), thisObj() );
			session->setOwner( thisObj() );
			session->start();
			session->asyncRead();
		}

		startAccept();
	}

	void cbTmr();
	void startTmr();
	//void run();

	// interface with session
	void broadcastMsg( const TcpSessionPtr& sess, const string& str);
	void deleteSession( const TcpSessionPtr& sess );

    uint64_t rx() { return m_rx;}
    uint64_t tx() { return m_tx;}
    uint64_t prevRx() { return m_prevRx;}
    uint64_t prevTx() { return m_prevTx;}
    void addRx( uint64_t rx ) { m_rx += rx ;}
    void addTx( uint64_t tx ) { m_tx += tx ;}
	*/

private:
	using strand = boost::asio::io_service::strand;

	ConnectHandler _connectHandler;
	ReadHandler _readHandler;
	ErrorHandler _errorHandler;

	State _state;
	Ptr<IoServicePool> _iosPool;

	std::mutex _mutex;
	std::deque<long>	_freeSessionId;
	std::map<long, Ptr<TcpSession>> _sessions;

	std::unique_ptr<strand> _strand;
	std::unique_ptr<tcp::acceptor> _acceptor;
	std::unique_ptr<tcp::socket> _socket;

	int 		_numThreads;
	IoMode 		_mode;

	/*
    uint64_t m_rx;
    uint64_t m_tx;
    uint64_t m_prevRx;
    uint64_t m_prevTx;
	*/
};
