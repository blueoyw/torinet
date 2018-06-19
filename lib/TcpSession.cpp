#include "TcpSession.h"
namespace tori{
namespace net{
TcpSession::TcpSession( asio::io_service& ios, UniquePtr<tcp::socket> socket, int id, ServerConfig& config )
	: _socket( move(socket) )
	  , _id ( id )
	  , _state ( State::Ready )
{
	//_strand(ios);
	//for reuse ip and port
	_socket->set_option(asio::socket_base::reuse_address(true));
	//for sending packet no delay
	_socket->set_option(tcp::no_delay( config._noDelay ));
}

bool TcpSession::getRemoteEndpoint( string& ip, uint16_t& port ) const
{
	if ( _state == State::Closed )
		return false;
	ip = _remoteEndpoint.address().to_string();
	port = _remoteEndpoint.port();
	return true;
}

void TcpSession::send( const uint8_t* data, size_t size )
{
	//TO DO
}

void TcpSession::send ( const Msg& msg )
{
    bool progress = !_sendQue.empty();
    _sendQue.push_back(msg);
    _sendMsg = msg;
    if( !progress ) 
    {
		boost::asio::async_write( *_socket,
				asio::buffer( &_sendQue.front(), _sendQue.front().length()),
				[this, self = shared_from_this()] ( error_code const& ec, std::size_t ) 
				{
					handleWrite(ec);
				});

		return;
		/*
        boost::asio::async_write(m_socket,
                boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                _strand.wrap(
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) );
		*/
    }

}

void TcpSession::send( Msg& msg )
{
	//TO DO

}

void TcpSession::handleWrite(const error_code& error)
{
    if (!error) 
    {
        _tx += _sendQue.front().length();
        //LOG(L_TRC, "ywoh test[%s] add tx byte[%lu] total[%lu]", lfunc, m_sendQue.front().length(), m_tx );
        _sendQue.pop_front();
        if( !_sendQue.empty() ) 
        {
			boost::asio::async_write( *_socket,
					asio::buffer( &_sendQue.front(), _sendQue.front().length()),
					[this, self = shared_from_this()] ( error_code const& ec, std::size_t ) 
					{
						handleWrite(ec);
					});
			/*
            boost::asio::async_write(m_socket,
                    boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length()),
                    m_strand.wrap(
                        boost::bind(&TcpSession::handleWrite, shared_from_this(),
                            boost::asio::placeholders::error) ) );
			*/
        }

    } 
    else 
    {
        //m_handler->error( error, shared_from_this(), m_owner );
    }
}

void TcpSession::start()
{

}

void TcpSession::close()
{
	//m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	_socket->close();
	/*
	LOG(L_DEB, "[%s] use_count[%d]", __func__
				, shared_from_this().use_count() );
	*/
}

bool TcpSession::isOpen() const
{

}

#if 0
void TcpSession::asyncSend( Msg& msg )
{
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

#endif

}
}



