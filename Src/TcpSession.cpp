#include "TcpSession.h"
namespace tori{
namespace net{
TcpSession::TcpSession( UniquePtr<tcp::socket> socket, int id, ServerConfig& config )
	: _socket( move(socket) )
	  , _id ( id )
	  , _state ( State::Ready )
{
    _minReceive = config._minReceive;
    _maxReceiveBufferSize = config._maxReceiveBufferSize;

	//_strand(ios);
    _remoteEndpoint = _socket->remote_endpoint();
	//for reuse ip and port
	_socket->set_option(asio::socket_base::reuse_address(true));
	//for sending packet no delay
	_socket->set_option(tcp::no_delay( config._noDelay ));
    //TO DO init config
}

bool TcpSession::getRemoteEndpoint( string& ip, uint16_t& port ) const
{
	if ( _state == State::Closed )
		return false;
	ip = _remoteEndpoint.address().to_string();
	port = _remoteEndpoint.port();
	return true;
}

void TcpSession::start()
{
    _state = State::Opening;
    //TO DO allocate buffer

    _state = State::Opened;

    _openedHandler(shared_from_this());

    read( Msg::hdrLength );
}

void TcpSession::read( size_t size )
{
    //boost::asio::async_read( _socket
    _socket->async_read_some(
                             boost::asio::buffer(_msg.head(), size )
                            , [this, self=shared_from_this()]
                            ( const error_code& ec, size_t  ) 
                            {
                                handleReadHeader( ec );
                            }
                            ) ;
}

void TcpSession::handleReadHeader( const error_code& error )
{
    if( error )
    {
        CloseReason reason = CloseReason::ActiveClose;
        if ( error == asio::error::eof || error == asio::error::operation_aborted )
            reason = CloseReason::Disconnected;
        handleError( error );
        _closedHandler( shared_from_this(), reason);
        return;
    }

	if (!error) 
    {
		cpMsgHeader_t* head = _msg.head();
		if( !_msg.checkHeader() ) 
        {
			LOG(L_WAR, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );

            read( Msg::hdrLength );
		} else {
            /*
			LOG(L_DEB, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );
                    */

			cpMsgHeader_t* hdr = _msg.head();

            //boost::asio::async_read( _socket
            _socket->async_read_some(
                                    boost::asio::buffer(_msg.body(), _msg.bodyLength() )
                                    , [this, self=shared_from_this()]
                                    (  const error_code& ec, std::size_t bytes ) 
                                    {
                                        cpMsgHeader_t* head = _msg.head();
                                        handleReadBody( head->msgId, head->bodyLength, ec );
                                    }
                                   ) ;
		}
	}
}

void 	TcpSession::handleReadBody( uint32_t& msgId, int16_t& length, const error_code& ec)
{
    if( ec )
    {
        CloseReason reason = CloseReason::ActiveClose;
        if ( ec == asio::error::eof || ec == asio::error::operation_aborted )
            reason = CloseReason::Disconnected;
        handleError( ec );
        _closedHandler( shared_from_this(), reason);
        return;
    }
    else 
    {
		//LOG(L_DEB, "msgId[%x]", msgId );

        _rx += _msg.length();
        //TO DO
        /*
		if( !_handler->read(msgId, length, shared_from_this(), m_owner ) ) 
			LOG(L_ERR, "[%s] process_msg fail", __func__ );
        */

        read( Msg::hdrLength );
	}
}

void TcpSession::handleError( const error_code& ec )
{
    LOG(L_ERR, "%s", ec.message().c_str() );
}

void TcpSession::send( const uint8_t* data, size_t size )
{
	//TO DO
}

void TcpSession::send ( const Msg& msg )
{
    bool progress = !_sendQue.empty();
    _sendQue.push_back(msg);
    if( !progress ) 
    {
		boost::asio::async_write( *_socket,
				asio::buffer( &_sendQue.front(), _sendQue.front().length()),
				[this, self = shared_from_this()] ( error_code const& ec, std::size_t ) 
				{
					handleWrite(ec);
				});

		return;
    }

}

void TcpSession::send( Msg& msg )
{
    bool progress = !_sendQue.empty();
    _sendQue.push_back(msg);
    if( !progress ) 
    {
		boost::asio::async_write( *_socket,
				asio::buffer( &_sendQue.front(), _sendQue.front().length()),
				[this, self = shared_from_this()] (  error_code const& ec, std::size_t ) 
				{
					handleWrite(ec);
				});

		return;
    }
}

void TcpSession::handleWrite( const error_code& ec)
{
    if( ec )
    {
        CloseReason reason = CloseReason::ActiveClose;
        if ( ec == asio::error::eof || ec == asio::error::operation_aborted )
            reason = CloseReason::Disconnected;
        handleError( ec );
        _closedHandler( shared_from_this(), reason);
        return;
    }
    else
    {
        _tx += _sendQue.front().length();
        //LOG(L_TRC, "ywoh test[%s] add tx byte[%lu] total[%lu]", lfunc, m_sendQue.front().length(), m_tx );
        _sendQue.pop_front();
        if( !_sendQue.empty() ) 
        {
			boost::asio::async_write( *_socket,
					asio::buffer( &_sendQue.front(), _sendQue.front().length()),
					[this, self = shared_from_this()] ( const error_code& ec, std::size_t ) 
					{
						handleWrite(ec);
					});
        }

    } 
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
    if( _state != State::Opened )
        return false;
    return true;
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



