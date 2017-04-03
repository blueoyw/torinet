#include "Session.h"
#include "Server.h"
void Session::start()
{
	//ywoh test
	//socket_.set_option(boost::asio::socket_base::reuse_address(true));
	
	boost::asio::async_read(socket_,
			boost::asio::buffer(msg_.head(), Msg::hdrLength),
			strand_.wrap(
				boost::bind(
					&Session::handle_read_header, shared_from_this(),
					boost::asio::placeholders::error)) );
}

void Session::close()
{
	//socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket_.close();
	msgTmr_.cancel();
	LOG(L_DEB, "[%s] use_count[%d]", __func__
				, shared_from_this().use_count() );
}

void Session::send( const Msg& msg )
{
    bool progress = !m_sendQue.empty();
    m_sendQue.push_back(msg);
    if( !progress )
    {
        boost::asio::async_write(socket_,
                boost::asio::buffer( &m_sendQue.front(), m_sendQue.front().length() ),
                strand_.wrap(
                    boost::bind(&Session::handle_write, shared_from_this(),
                        boost::asio::placeholders::error) ) );
    }
}

#if 0
void Session::broadcast( const string& str)
{
	if( !server_.expired() ) {
		Server_ptr server = server_.lock();
		if( server )
			server->broadcast_msg( shared_from_this(), str);
	}
}
#endif

void Session::delete_session()
{
	close();
	if( !server_.expired() ) {
		Server_ptr server = server_.lock();
		if( server ) {
			server->delete_session( shared_from_this() );
		}
	}
}

void Session::handle_read_header(const boost::system::error_code& error)
{
	if (!error) {
		//to do check msg id
		cpMsgHeader_t* head = msg_.head();
		if( !msg_.checkHeader() ) {
			//send_msg_->encodeMsg( rsCrteUser, INVALID_MSG, NULL, 0);
			//send_msg();
			//return;
			LOG(L_WAR, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );
		}
         
        /*
		LOG(L_DEB, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
				, head->result , head->bodyLength );
                */

		cpMsgHeader_t* hdr = msg_.head();
		boost::asio::async_read(socket_,
				boost::asio::buffer(msg_.body(), msg_.bodyLength()),
				strand_.wrap(
					boost::bind(&Session::handle_read_body, shared_from_this(), hdr->msgId
						, hdr->bodyLength , boost::asio::placeholders::error)) ) ;
	} else {
		delete_session();
	}
}

void Session::handle_read_body(uint32_t& msgId, int16_t& length, const boost::system::error_code& error)
{
	if (!error) 
    {
		//LOG(L_DEB, "msgId[%x]", msgId );

		//process_msg(msgId);
		//if( cb_ && (!cb_(msgId, length, msg_.body(), send_msg_) ) )
		if( cb_ && (!cb_(msgId, length, (void*)&shared_from_this() ) ) )
			LOG(L_ERR, "[%s] process_msg fail", __func__ );

		boost::asio::async_read(socket_,
				boost::asio::buffer(msg_.head(), Msg::hdrLength),
				strand_.wrap(
					boost::bind(
						&Session::handle_read_header, shared_from_this(),
						boost::asio::placeholders::error)) ) ;
		//std::string str("server response");
		//send_msg(str);
		//broadcast(str);
		send();
	} 
    else 
    {
		delete_session();
	}
}

#if 0
void Session::process_msg(uint32_t& msgId)
{
	LOG(L_DEB, "msgId[%x] %s", msgId, msg_.body() );

	bool	rtn = false;
	
	//ywoh to do
	send_msg();
}
#endif

void Session::handle_write(const boost::system::error_code& error)
{
	if (!error) {

	} else {
		delete_session();
	}
}


