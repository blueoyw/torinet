#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_
#include "Incl.h"
#include "IoServicePool.h"
#include "TcpSession.h"

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using namespace std;

typedef std::set<TcpSessionPtr> TcpSessionSet;
typedef std::set<TcpSessionPtr>::iterator TcpSessionItor;

class TcpServer
:public boost::enable_shared_from_this<TcpServer>
{
public:
	TcpServer(boost::asio::io_service& ios,
			const tcp::endpoint& endpoint, size_t numThreads,
			//SessionHandler* handler
			SessionHandlerPtr handler
			)
		: m_ios(ios),
		m_acceptor(ios, endpoint), m_tmr(ios),
		m_handler(handler),
		m_numThreads(numThreads), m_ioMode(SINGLE_ASIO), m_rx(0), m_tx(0)
        ,m_prevRx(0), m_prevTx(0)

	{
		// problem for shared_from_this in constructor
		// handle_signal();
		//startAccept();
		//startTmr();
	}

	TcpServer(
			const tcp::endpoint& endpoint, size_t numThreads,
			SessionHandlerPtr handler, IoServicePoolPtr iosPool
			)
		: m_ios(iosPool->getIoService()),
		m_acceptor(m_ios, endpoint), m_tmr(m_ios),
		m_handler(handler),
		m_numThreads(numThreads), m_ioMode(MULTI_ASIO), m_pIosPool(iosPool)
        , m_rx(0), m_tx(0)
        ,m_prevRx(0), m_prevTx(0)
	{
		// problem for shared_from_this in constructor
		// handle_signal();
		//startAccept();
		//startTmr();
	}

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
		if ( m_ioMode == MULTI_ASIO ) {
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

private:
	boost::asio::io_service& 	m_ios;
	tcp::acceptor 					m_acceptor;
	TcpSessionSet					m_sessions;
	deadline_timer					m_tmr;
	bool								m_isDaemon;
	SessionHandlerPtr				m_handler;;
	size_t 							m_numThreads;
	IoMode 							m_ioMode;
	IoServicePoolPtr				m_pIosPool;

    uint64_t m_rx;
    uint64_t m_tx;
    uint64_t m_prevRx;
    uint64_t m_prevTx;
};

typedef boost::shared_ptr<TcpServer> TcpServerPtr;
typedef boost::weak_ptr<TcpServer> TcpServerWeakPtr;


#endif	

