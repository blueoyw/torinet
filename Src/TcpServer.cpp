#include "TcpServer.h"

namespace tori{
namespace net{

	
TcpServer::TcpServer(const string name, const ServerConfig& config)
    : _name(name), _config(config), _state(State::Ready)
      , _ioServicePool( new IoServicePool(config._threadCount) )
{
    for( int i=0; i<config._maxSession; i++ ) {
        _free_session_id.push_back(i+1);
    }

}

void TcpServer::start(uint16_t port)
{
    tcp::endpoint ep( tcp::v4(), port);
    listen( ep );
}

void TcpServer::start(std::string address, uint16_t port)
{
    tcp::endpoint ep( asio::ip::address::from_string(address), port);
    listen( ep );
}

void TcpServer::stop()
{
    if( _state != State::Start) 
        return;

    _acceptor->close();

    for( auto& pair : _sessions )
    {
        pair.second->close();
    }
    _sessions.clear();
    _free_session_id.clear();

    _state = State::Stop;
}

inline void TcpServer::handleClose( const Ptr<TcpSession>& session, CloseReason reason )
{
    if( _closedHandler )
        _closedHandler( session, reason );
    int id = session->getID();
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _sessions.erase( id );
    }
    _free_session_id.push_back( id );
}

void TcpServer::listen(tcp::endpoint ep)
{
    UniquePtr<tcp::acceptor> acceptor( new tcp::acceptor( _ioServicePool->getIoService() ) );
    acceptor->open( ep.protocol() );
    acceptor->set_option( tcp::acceptor::reuse_address(true));
    acceptor->bind( ep );
    acceptor->listen( tcp::acceptor::max_connections );

    _acceptor = std::move(acceptor);

    _state = State::Start;
    startAccept();
}

void TcpServer::startAccept()
{
    LOG (L_INF, "start [%s]", __func__);
    UniquePtr<tcp::socket> socket(new tcp::socket(_ioServicePool->getIoService()));
    _socket = std::move(socket);
    _acceptor->async_accept( *_socket,
                [this, self=shared_from_this()]( error_code error )
                {
                    if(error ) 
                    {
                        LOG(L_ERR, "%s", error.message().c_str()); 
                        return;
                    }

                    if( _state == State::Stop ||
                       _sessions.size() >= _config._maxSession) 
                    {
                        error_code ec;
                        _socket->shutdown(tcp::socket::shutdown_both, ec );
                        _socket->close();
                        _socket.reset();
                        return;
                    }
                    else if (!error )
                    {
                        int id = _free_session_id.front();
                        _free_session_id.pop_front();

                        auto session = std::make_shared<TcpSession> ( std::move(_socket), id, _config );
                        session->_openedHandler = _openedHandler;
                        session->_closedHandler = [this]
                            ( const Ptr<TcpSession>& session, CloseReason closeReason ) 
                            {
                                this->handleClose( session, closeReason );
                            };
                        session->_messageHandler = _messageHandler;

                        {
                            std::lock_guard<std::mutex> guard(_mutex);
                            _sessions.emplace( make_pair( session->getID(), session ));
                        }
                        session->start();
                    }

                    startAccept();
                });
    _ioServicePool->run();
}

#if 0
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
#endif

}
}
