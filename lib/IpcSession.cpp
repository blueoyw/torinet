#include "IpcSession.h"
void IpcSession::start()
{
	//for reuse ip and port
	//m_socket.set_option(boost::asio::socket_base::reuse_address(true));
	//for sending packet no delay
	//m_socket.set_option(boost::asio::ip::tcp::no_delay(true));
}

void IpcSession::close()
{
	//m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	m_socket.close();
	m_msgTmr.cancel();
	LOG(L_DEB, "[%s] use_count[%d]", __func__
				, shared_from_this().use_count() );
}

void IpcSession::asyncSend()
{
	boost::asio::async_write(m_socket,
			boost::asio::buffer(m_sendMsg.data(), m_sendMsg.length()),
			m_strand.wrap(
				boost::bind(&IpcSession::handleWrite, shared_from_this(),
					boost::asio::placeholders::error) ) );
}

void IpcSession::asyncConnect()
{
	m_socket.async_connect( m_epRemote, boost::bind(&IpcSession::handleConnect,
				               shared_from_this(), boost::asio::placeholders::error) );
}

void IpcSession::asyncRead()
{
	boost::asio::async_read(m_socket
			, boost::asio::buffer(m_msg.head(), Msg::hdrLength)
			, m_strand.wrap(
				boost::bind(
					&IpcSession::handleReadHeader, shared_from_this(),
					boost::asio::placeholders::error)) );
}

void IpcSession::handleConnect(const boost::system::error_code& error)
{
	if (!error) {
		m_handler->connect( shared_from_this(), m_owner );
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}

void IpcSession::handleReadHeader(const boost::system::error_code& error)
{
	if (!error) {
		//to do check msg id
		cpMsgHeader_t* head = m_msg.head();
		if( !m_msg.checkHeader() ) {
			//m_sendMsg->encodeMsg( rsCrteUser, INVALID_MSG, NULL, 0);
			//send_msg();
			//return;
			LOG(L_WAR, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
					, head->result , head->bodyLength );
		}
		LOG(L_DEB, "[%s] id[%d] result[%d] length[%d]", __func__, head->msgId
				, head->result , head->bodyLength );

		cpMsgHeader_t* hdr = m_msg.head();
		boost::asio::async_read(m_socket,
				boost::asio::buffer(m_msg.body(), m_msg.bodyLength()),
				m_strand.wrap(
					boost::bind(&IpcSession::handleReadBody, shared_from_this(), hdr->msgId
						, hdr->bodyLength , boost::asio::placeholders::error)) ) ;
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}

void IpcSession::handleReadBody(uint32_t& msgId, int16_t& length, const boost::system::error_code& error)
{
	if (!error) {
		LOG(L_DEB, "msgId[%x]", msgId );

		if( !m_handler->read(msgId, length, shared_from_this(), m_owner ) )
			LOG(L_ERR, "[%s] process_msg fail", __func__ );

		boost::asio::async_read(m_socket,
				boost::asio::buffer(m_msg.head(), Msg::hdrLength),
				m_strand.wrap(
					boost::bind(
						&IpcSession::handleReadHeader, shared_from_this(),
						boost::asio::placeholders::error)) ) ;
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}

void IpcSession::handleWrite(const boost::system::error_code& error)
{
	if (!error) {

	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}


