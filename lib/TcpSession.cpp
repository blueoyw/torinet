#include "TcpSession.h"
TcpSession::TcpSession(long& id, std::unique_ptr<tcp::socket> socket, const IoMode& ioMode )
: _id(id), _socket(std::move(socket)), _ioMode(ioMode), _tx(0), _rx(0), _state(State::Ready)
{
	_strand.reset( new asio::io_service::strand( _socket->get_io_service() ) );
	//gcc 4.8 not support c++14
	//_strand = std::make_unique<asio::io_service::strand>( _socket->get_io_service() );
	_remoteEndpoint = _socket->remote_endpoint();
}

bool TcpSession::getRemoteEndpoint( std::string& ip, uint16_t& port )
{
	if( _state == State::Closed)
		return false;
	ip = _remoteEndpoint.address().to_string();
	port = _remoteEndpoint.port();
	return true;
}

void TcpSession::send( const uint8_t* data, size_t size )
{

}

void send( const Msg& buf )
{

}

void read()
{

}

void start()
{

}

void close()
{

}

bool isOpen()
{

}

/*
asio::strand getStrand()
{

}
*/

#if 0
void TcpSession::start()
{
	//for reuse ip and port
	_socket.set_option(boost::asio::socket_base::reuse_address(true));
	//for sending packet no delay
	_socket.set_option(boost::asio::ip::tcp::no_delay(true));
}

void TcpSession::close()
{
	//_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	_socket.close();
	_msgTmr.cancel();
	LOG(L_DEB, "[%s] use_count[%d]", __func__
				, shared_from_this().use_count() );
}

void TcpSession::asyncSend( Msg& msg )
{
    bool progress = !_sendQue.empty();
    _sendQue.push_back(msg);
    _sendMsg = msg;
    if( !progress ) 
    {
        if ( _ioMode == MULTI_ASIO ) 
        {
            boost::asio::async_write(_socket,
                    boost::asio::buffer( &_sendQue.front(), _sendQue.front().length()),
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) ;
            return;
        }
        boost::asio::async_write(_socket,
                boost::asio::buffer( &_sendQue.front(), _sendQue.front().length()),
                _strand.wrap(
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) );
    }
}

void TcpSession::asyncSend()
{
    bool progress = !_sendQue.empty();
    _sendQue.push_back(_sendMsg);
    if( !progress ) 
    {
        if ( _ioMode == MULTI_ASIO ) 
        {
            boost::asio::async_write(_socket,
                    boost::asio::buffer( &_sendQue.front(), _sendQue.front().length()),
                    boost::bind(&TcpSession::handleWrite, shared_from_this(),
                        boost::asio::placeholders::error) ) ;
            return;
        }
        boost::asio::async_write(_socket,
                boost::asio::buffer( &_sendQue.front(), _sendQue.front().length()),
                _strand.wrap(
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
	_socket.async_connect( _epRemote, boost::bind(&TcpSession::handleConnect,
				               shared_from_this(), boost::asio::placeholders::error) );
}

void TcpSession::asyncRead()
{
	if ( _ioMode == MULTI_ASIO ) {
		boost::asio::async_read(_socket
				, boost::asio::buffer(_msg.head(), Msg::hdrLength)
				,	boost::bind(
						&TcpSession::handleReadHeader, shared_from_this(),
						boost::asio::placeholders::error)) ;
		return;
	}
	boost::asio::async_read(_socket
			, boost::asio::buffer(_msg.head(), Msg::hdrLength)
			, _strand.wrap(
				boost::bind(
					&TcpSession::handleReadHeader, shared_from_this(),
					boost::asio::placeholders::error)) );
}

void TcpSession::handleConnect(const boost::system::error_code& error)
{
	if (!error) {
		_handler->connect( shared_from_this(), _owner );
	} else {
		_handler->error( error, shared_from_this(), _owner );
	}
}

void TcpSession::handleReadHeader(const boost::system::error_code& error)
{
	if (!error) {
		cpMsgHeader_t* head = _msg.head();
		if( !_msg.checkHeader() ) 
        {
			LOG(L_WAR, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );

			if ( _ioMode == MULTI_ASIO ) {
				boost::asio::async_read(_socket,
						boost::asio::buffer(_msg.head(), Msg::hdrLength),
							boost::bind(
								&TcpSession::handleReadHeader, shared_from_this(),
								boost::asio::placeholders::error)) ;
				return;
			}
			boost::asio::async_read(_socket,
					boost::asio::buffer(_msg.head(), Msg::hdrLength),
					_strand.wrap(
						boost::bind(
							&TcpSession::handleReadHeader, shared_from_this(),
							boost::asio::placeholders::error)) ) ;
		} else {
            /*
			LOG(L_DEB, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );
                    */

			cpMsgHeader_t* hdr = _msg.head();

			if ( _ioMode == MULTI_ASIO ) {
				boost::asio::async_read(_socket,
						boost::asio::buffer(_msg.body(), _msg.bodyLength()),
							boost::bind(&TcpSession::handleReadBody, shared_from_this(), hdr->msgId
								, hdr->bodyLength , boost::asio::placeholders::error)) ;
				return;
			}
			boost::asio::async_read(_socket,
					boost::asio::buffer(_msg.body(), _msg.bodyLength()),
					_strand.wrap(
						boost::bind(&TcpSession::handleReadBody, shared_from_this(), hdr->msgId
							, hdr->bodyLength , boost::asio::placeholders::error)) ) ;
		}
	} else {
		_handler->error( error, shared_from_this(), _owner );
	}
}

void TcpSession::handleReadBody(uint32_t& msgId, int16_t& length, const boost::system::error_code& error)
{
	if (!error) {
		//LOG(L_DEB, "msgId[%x]", msgId );

        _rx += _msg.length();
		if( !_handler->read(msgId, length, shared_from_this(), _owner ) ) 
			LOG(L_ERR, "[%s] process_msg fail", __func__ );

		if ( _ioMode == MULTI_ASIO ) 
        {
            boost::asio::async_read(_socket,
                    boost::asio::buffer(_msg.head(), Msg::hdrLength),
                    boost::bind(
                        &TcpSession::handleReadHeader, shared_from_this(),
                        boost::asio::placeholders::error)) ;
			return;
		}
		boost::asio::async_read(_socket,
				boost::asio::buffer(_msg.head(), Msg::hdrLength),
				_strand.wrap(
					boost::bind(
						&TcpSession::handleReadHeader, shared_from_this(),
						boost::asio::placeholders::error)) ) ;
	} else {
		_handler->error( error, shared_from_this(), _owner );
	}
}

void TcpSession::handleWrite(const boost::system::error_code& error)
{
    if (!error) 
    {
        _tx += _sendQue.front().length();
        //LOG(L_TRC, "ywoh test[%s] add tx byte[%lu] total[%lu]", lfunc, _sendQue.front().length(), _tx );
        _sendQue.pop_front();
        if( !_sendQue.empty() ) 
        {
            if ( _ioMode == MULTI_ASIO ) 
            {
                boost::asio::async_write(_socket,
                        boost::asio::buffer( &_sendQue.front(), _sendQue.front().length()),
                        boost::bind(&TcpSession::handleWrite, shared_from_this(),
                            boost::asio::placeholders::error) ) ;
                return;
            }
            boost::asio::async_write(_socket,
                    boost::asio::buffer( &_sendQue.front(), _sendQue.front().length()),
                    _strand.wrap(
                        boost::bind(&TcpSession::handleWrite, shared_from_this(),
                            boost::asio::placeholders::error) ) );
        }

    } 
    else 
    {
        _handler->error( error, shared_from_this(), _owner );
    }
}
#endif
