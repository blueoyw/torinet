#include "TcpServer.h"
TcpServer::TcpServer( size_t numThreads, IoMode mode )
		:_numThreads(numThreads), _mode(mode)
{
	_iosPool = std::make_shared<IoServicePool>( _numThreads, _mode );
}

void TcpServer::start(uint16_t port)
{

}

void TcpServer::start(std::string ip, uint16_t port)
{

}

void TcpServer::stop()
{

}

Ptr<ISession> TcpServer::getSession( long id )
{

}

void TcpServer::listen( tcp::endpoint endpoint )
{
}

void TcpServer::startAccept()
{

}

/*
void TcpServer::cbTmr()
{
	LOG(L_FAT, "[%s] ############ sessions = %ld ###########", lfunc, m_sessions.size() );
	LOG(L_FAT, "[%s] avg rx[%f]Gbps tx[%f]Gbps", lfunc, double(m_rx-m_prevRx)*8/60/1024/1024, double(m_tx-m_prevTx)*8/60/1024/1024 );

    m_prevRx = m_rx;
    m_prevTx = m_tx;

	if( m_tmr.expires_at() <= deadline_timer::traits_type::now() ) 
    {
		m_tmr.expires_from_now(boost::posix_time::seconds(60));
		m_tmr.async_wait( boost::bind(&TcpServer::cbTmr, shared_from_this() )) ;
		return;
	}
}

void TcpServer::startTmr()
{
	m_tmr.expires_from_now(boost::posix_time::seconds(60));
	m_tmr.async_wait( boost::bind(&TcpServer::cbTmr, shared_from_this() ) ) ;
}

void TcpServer::broadcastMsg( const TcpSessionPtr& sess, const string& str )
{
#if 0
	Session_itor itor = m_sessions.begin();
	for( ; itor!=m_sessions.end(); itor++) {
		if ( sess == *itor ) 
			continue;
		else {
			(*itor)->send_msg(str);
		}
	}
#endif
}

void TcpServer::deleteSession( const TcpSessionPtr& sess )
{
	LOG(L_INF,"session[%d] delete", sess->socket().native() );
	sess->close();
	m_sessions.erase( sess );
	LOG(L_DEB, "[%s] use_count[%d]", __func__, sess.use_count() );
}
*/
