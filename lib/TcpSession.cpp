#include "TcpSession.h"
namespace tori{
namespace net{
TcpSession::TcpSession( UniquePtr(tcp::socket) socket, int id, IoMode& ioMode )
	: _socket( std::move(socket) )
	  , _id ( id )
	  , _state ( State::Ready )
	  , _ioMode ( ioMode )
{
	UniquePtr( asio::io_service::strand ) p1( new asio::io_service::strand( _socket->get_io_service() );
	_strand = move(p1 );

}

bool TcpSession::getRemoteEndpoint( string& ip, uint16_t& port ) const
{
	if ( _state == State::Closed )
		return false;
}

void TcpSession::send( const uint8_t* data, size_t size )
{

}

void TcpSession::send ( const Msg& msg )
{

}

void TcpSession::send( Msg& msg )
{

}

void TcpSession::start()
{

}

void TcpSession::close()
{

}

bool TcpSession::isOpen() const
{

}

#if 0
void TcpSession::start()
{
	//for reuse ip and port
	m_socket.set_option(boost::asio::socket_base::reuse_address(true));
	//for sending packet no delay
	m_socket.set_option(boost::asio::ip::tcp::no_delay(true));
}

void TcpSession::close()
{
	//m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	m_socket.close();
	m_msgTmr.cancel();
	LOG(L_DEB, "[%s] use_count[%d]", __func__
				, shared_from_this().use_count() );
}

void TcpSession::asyncSend( Msg& msg )
{
    bool progress = !m_sendQue.empty();
    m_sendQue.push_back(msg);
    m_sendMsg = msg;
    if( !progress ) 
    {
        if ( m_ioMode == MULTI_ASIO ) 
        {
            boost::asio::async_write(m_socket,
                    boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) ;
            return;
        }
        boost::asio::async_write(m_socket,
                boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                m_strand.wrap(
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) );
    }
}

void TcpSession::asyncSend()
{
    bool progress = !m_sendQue.empty();
    m_sendQue.push_back(m_sendMsg);
    if( !progress ) 
    {
        if ( m_ioMode == MULTI_ASIO ) 
        {
            boost::asio::async_write(m_socket,
                    boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) ;
            return;
        }
        boost::asio::async_write(m_socket,
                boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                m_strand.wrap(
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) );
    }
}

#if 0
void TcpSession::broadcast( const string& str)
{
	if( !server_.expired() ) {
		Server_ptr server = server_.lock();
		if( server )
			server->broadcast_msg( shared_from_this(), str);
	}
}
#endif

void TcpSession::asyncConnect()
{
	m_socket.async_connect( m_epRemote, boost::bind(&TcpSession::handleConnect,
				               shared_from_this(), boost::asio::placeholders::error) );
}

void TcpSession::asyncRead()
{
	if ( m_ioMode == MULTI_ASIO ) {
		boost::asio::async_read(m_socket
				, boost::asio::buffer(m_msg.head(), Msg::hdrLength)
				,	boost::bind(
						&TcpSession::handleReadHeader, shared_from_this(),
						boost::asio::placeholders::error)) ;
		return;
	}
	boost::asio::async_read(m_socket
			, boost::asio::buffer(m_msg.head(), Msg::hdrLength)
			, m_strand.wrap(
				boost::bind(
					&TcpSession::handleReadHeader, shared_from_this(),
					boost::asio::placeholders::error)) );
}

void TcpSession::handleConnect(const boost::system::error_code& error)
{
	if (!error) {
		m_handler->connect( shared_from_this(), m_owner );
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}

void TcpSession::handleReadHeader(const boost::system::error_code& error)
{
	if (!error) {
		cpMsgHeader_t* head = m_msg.head();
		if( !m_msg.checkHeader() ) 
        {
			LOG(L_WAR, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );

			if ( m_ioMode == MULTI_ASIO ) {
				boost::asio::async_read(m_socket,
						boost::asio::buffer(m_msg.head(), Msg::hdrLength),
							boost::bind(
								&TcpSession::handleReadHeader, shared_from_this(),
								boost::asio::placeholders::error)) ;
				return;
			}
			boost::asio::async_read(m_socket,
					boost::asio::buffer(m_msg.head(), Msg::hdrLength),
					m_strand.wrap(
						boost::bind(
							&TcpSession::handleReadHeader, shared_from_this(),
							boost::asio::placeholders::error)) ) ;
		} else {
            /*
			LOG(L_DEB, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );
                    */

			cpMsgHeader_t* hdr = m_msg.head();

			if ( m_ioMode == MULTI_ASIO ) {
				boost::asio::async_read(m_socket,
						boost::asio::buffer(m_msg.body(), m_msg.bodyLength()),
							boost::bind(&TcpSession::handleReadBody, shared_from_this(), hdr->msgId
								, hdr->bodyLength , boost::asio::placeholders::error)) ;
				return;
			}
			boost::asio::async_read(m_socket,
					boost::asio::buffer(m_msg.body(), m_msg.bodyLength()),
					m_strand.wrap(
						boost::bind(&TcpSession::handleReadBody, shared_from_this(), hdr->msgId
							, hdr->bodyLength , boost::asio::placeholders::error)) ) ;
		}
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}

void TcpSession::handleReadBody(uint32_t& msgId, int16_t& length, const boost::system::error_code& error)
{
	if (!error) {
		//LOG(L_DEB, "msgId[%x]", msgId );

        m_rx += m_msg.length();
		if( !m_handler->read(msgId, length, shared_from_this(), m_owner ) ) 
			LOG(L_ERR, "[%s] process_msg fail", __func__ );

		if ( m_ioMode == MULTI_ASIO ) 
        {
            boost::asio::async_read(m_socket,
                    boost::asio::buffer(m_msg.head(), Msg::hdrLength),
                    boost::bind(
                        &TcpSession::handleReadHeader, shared_from_this(),
                        boost::asio::placeholders::error)) ;
			return;
		}
		boost::asio::async_read(m_socket,
				boost::asio::buffer(m_msg.head(), Msg::hdrLength),
				m_strand.wrap(
					boost::bind(
						&TcpSession::handleReadHeader, shared_from_this(),
						boost::asio::placeholders::error)) ) ;
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}

#if 0
void TcpSession::process_msg(uint32_t& msgId)
{
	LOG(L_DEB, "msgId[%x] %s", msgId, m_msg.body() );

	bool	rtn = false;
	
	//ywoh to do
	send_msg();
}
#endif

void TcpSession::handleWrite(const boost::system::error_code& error)
{
    if (!error) 
    {
        m_tx += m_sendQue.front().length();
        //LOG(L_TRC, "ywoh test[%s] add tx byte[%lu] total[%lu]", lfunc, m_sendQue.front().length(), m_tx );
        m_sendQue.pop_front();
        if( !m_sendQue.empty() ) 
        {
            if ( m_ioMode == MULTI_ASIO ) 
            {
                boost::asio::async_write(m_socket,
                        boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                        boost::bind(&TcpSession::handleWrite, shared_from_this(),
                            boost::asio::placeholders::error) ) ;
                return;
            }
            boost::asio::async_write(m_socket,
                    boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                    m_strand.wrap(
                        boost::bind(&TcpSession::handleWrite, shared_from_this(),
                            boost::asio::placeholders::error) ) );
        }

    } 
    else 
    {
        m_handler->error( error, shared_from_this(), m_owner );
    }
}
#endif

}
}



