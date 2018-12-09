#pragma once

#include "Incl.h"

class ClientHandler: public SessionHandler
{
public:
	ClientHandler(){}
	virtual ~ClientHandler(){}

	virtual bool   read( uint32_t& msgId, int16_t& length, VoidPtr sess, VoidWeakPtr owner ) ;
	virtual bool   error( const boost::system::error_code& error, VoidPtr sess, VoidWeakPtr owner );
	virtual bool   connect( VoidPtr sess, VoidWeakPtr owner );

};

typedef boost::shared_ptr<ClientHandler> ClientHandlerPtr;

extern bool g_bTerm;
class Client
:public boost::enable_shared_from_this<Client>
{
public:
	static const int MASTER_CLIENT = 0;

	Client( const string& host, const string& id ):
		 m_partyId(0), m_tx(0), m_rx(0), m_signals(m_ios), m_seq(0), m_id(id), m_monitorTmr(m_ios)
         , m_bTx(false), m_txTmr(m_ios), m_bRun(true)
	{
		m_pid = getpid();

        /*
		m_signals.add(SIGINT);
		m_signals.add(SIGTERM);
		m_signals.async_wait(boost::bind(&Client::stop, this) );
        */

		boost::asio::ip::tcp::endpoint ep(
				boost::asio::ip::address::from_string(host), 50000);

		ClientHandlerPtr	handler(new ClientHandler());
		TcpSessionPtr session(new TcpSession(m_ios, ep, handler));
		m_session = session;
	}

	void stop()
	{
		LOG(L_FAT,"[%s] %d-%s client stop", __func__, m_pid, m_id.c_str());
		m_ios.stop();	
	}

	void cbDeadline()
	{
		//expires 
		if( m_session->msgTmr().expires_at() <= deadline_timer::traits_type::now() ) {
            //test write data
            nf_Message_t cpMsg;
            cpMsg.partyId = 1;
            memset( cpMsg.msg, 0xAB, 64 );

            Msg msg;
	        msg.encodeMsg(nf_Message, (uint8_t*)&cpMsg, sizeof(cpMsg));
			write( msg );
			m_session->msgTmr().cancel();
			m_session->msgTmr().expires_from_now(boost::posix_time::seconds(1));
			m_session->msgTmr().async_wait(boost::bind(&Client::cbDeadline, this));
			return;
		}

		//don't expires
	}

	void sendMsg(const string& str) 
	{
#if 0
		m_session->send(str);
		m_session->msgTmr().cancel();
		m_session->msgTmr().expires_from_now(boost::posix_time::seconds(5));
		m_session->msgTmr().async_wait(boost::bind(&Client::cbDeadline, this));
#endif
	}

	void startTmr() 
	{
		//m_session->send();
		m_session->msgTmr().cancel();
		m_session->msgTmr().expires_from_now(boost::posix_time::seconds(1));
		m_session->msgTmr().async_wait(boost::bind(&Client::cbDeadline, this));
	}

	void cbMonitor( const boost::system::error_code& e )
	{
        if (e.value()) 
        {
            if (e.value() != boost::system::errc::operation_canceled) {
                LOG(L_ERR, "[%s] error_code=%s(%d)", __func__, e.message().c_str(), e.value());
            }
            return;
        }

		//don't expires
        /*
        if( t->expires_at() > boost::asio::deadline_timer::traits_type::now() ) 
        {
            return;
        }
        */

        //print packet stat
        double tmpTx = m_session->tx() - m_tx;
        double tmpRx = m_session->rx() - m_rx;
        LOG(L_FAT, "[%s] avg tx [%lu]bytes [%f]Gbps avg [%lu]bytes rx[%f]Gbps "
                , __func__
                , m_session->tx(), tmpTx*8/60/1024/1024
                , m_session->rx(), tmpRx*8/60/1024/1024 );

        m_tx = m_session->tx();
        m_rx = m_session->rx();

		m_monitorTmr.expires_from_now(boost::posix_time::seconds(60));
        m_monitorTmr.async_wait(boost::bind(&Client::cbMonitor, this
                    , boost::asio::placeholders::error));
	}

	void startMonitor() 
	{
        SLOG
		m_monitorTmr.expires_from_now(boost::posix_time::seconds(60));
        m_monitorTmr.async_wait(boost::bind(&Client::cbMonitor, this
                    , boost::asio::placeholders::error));
	}

	void cbTx( const boost::system::error_code& e, int count )
	{
        if (e.value()) 
        {
            if (e.value() != boost::system::errc::operation_canceled) 
            {
                LOG(L_ERR, "[%s] error_code=%s(%d)", __func__, e.message().c_str(), e.value());
            }
            return;
        }

		//don't expires
        /*
        if( t->expires_at() > boost::asio::deadline_timer::traits_type::now() ) 
        {
            return;
        }
        */

        if( m_bTx )
        {
            for( int i=0; i< count; i++ )
            {
                m_session->asyncSend();
            }

            m_txTmr.expires_from_now(boost::posix_time::millisec(10));
            m_txTmr.async_wait(boost::bind(&Client::cbTx, this
                        , boost::asio::placeholders::error, count));
        }
	}

	void startTx( int& count ) 
	{
        LOG(L_FAT, "[%s] %d count per 10 mili-second", lfunc, count);
        m_bTx = true;
		m_txTmr.expires_from_now(boost::posix_time::millisec(10));
        m_txTmr.async_wait(boost::bind(&Client::cbTx, this
                    , boost::asio::placeholders::error, count));
	}

    void stopTx()
    {
        m_bTx = false;
        m_txTmr.cancel();
    }

	void startRx() 
	{
        startMonitor();
		m_session->asyncConnect();
		m_ios.run();
		m_session->asyncRead();
	}

	void close()
	{
		// Cancel all outstanding asynchronous operations.
		m_session->close();
	}

	void write( Msg& msg)
	{
		//m_ios.post( boost::bind(&Client::doWrite, shared_from_this(), msg));
        m_session->asyncSend(msg);
	}

	//getter
	TcpSessionPtr		session(){ return m_session; }
	int					seq(){ return m_seq; }
	int					add_seq(){ return m_seq++; }
	string&				id() { return m_id; }
	uint32_t				m_partyId;

    long                        m_tx;
    long                        m_rx;
private:
	TcpSessionPtr				m_session;
	boost::asio::io_service     m_ios;
	int							m_pid;
	boost::asio::signal_set     m_signals;
	int							m_seq;
	string						m_id;

	deadline_timer	            m_monitorTmr;		
    bool                        m_bTx;
	deadline_timer	            m_txTmr;		
    bool                        m_bRun;
};

typedef boost::shared_ptr<Client>	ClientPtr;
typedef boost::weak_ptr<Client>		ClientWeakPtr;

typedef std::vector<ClientPtr> 				ClientVec;
typedef std::vector<ClientPtr>::iterator	ClientVecItor;
