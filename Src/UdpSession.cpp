#include "UdpSession.h"
void UdpSession::start()
{
	//for reuse ip and port
	m_socket.set_option(boost::asio::socket_base::reuse_address(true));
	
}

void UdpSession::close()
{
	//m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
	m_socket.close();
	m_msgTmr.cancel();
	LOG(L_DEB, "[%s] use_count[%d]", __func__
				, shared_from_this().use_count() );
}

void UdpSession::asyncConnect()
{
	m_socket.async_connect( m_epRemote, boost::bind(&UdpSession::handleConnect,
				shared_from_this(), boost::asio::placeholders::error) );
}

void UdpSession::asyncRead()
{
	if( m_udpType == UDP_CLIENT ) {
		m_socket.async_receive(
				boost::asio::buffer(m_msg.data(), sizeof(cpMsg_t)),
				m_strand.wrap(
					boost::bind(
						&UdpSession::handleRead, shared_from_this(),
						boost::asio::placeholders::error)) );
	} else 
		m_socket.async_receive_from(
				boost::asio::buffer(m_msg.data(), sizeof(cpMsg_t)), m_epRemote,
				m_strand.wrap(
					boost::bind(
						&UdpSession::handleRead, shared_from_this(),
						boost::asio::placeholders::error)) );
}

void UdpSession::asyncSend()
{
	if( m_udpType == UDP_CLIENT ) {
		m_socket.async_send(
				boost::asio::buffer(m_sendMsg.data(), m_sendMsg.length()),
				m_strand.wrap(
					boost::bind(&UdpSession::handleWrite, shared_from_this(),
						boost::asio::placeholders::error) ) );
	} else 
		m_socket.async_send_to(
				boost::asio::buffer(m_msg.data(), m_sendMsg.length()), m_epRemote,
				m_strand.wrap(
					boost::bind(
						&UdpSession::handleWrite, shared_from_this(),
						boost::asio::placeholders::error)) );
}

#if 0
void UdpSession::broadcast( const string& str)
{
	if( !server_.expired() ) {
		Server_ptr server = server_.lock();
		if( server )
			server->broadcast_msg( shared_from_this(), str);
	}
}
#endif


void UdpSession::handleConnect(const boost::system::error_code& error)
{
	if (!error) {
		m_handler->connect( shared_from_this(), m_owner );
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}
		

void UdpSession::handleRead(const boost::system::error_code& error)
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
		if( !m_handler->read(hdr->msgId, hdr->bodyLength, shared_from_this(), m_owner ) )
			LOG(L_ERR, "[%s] process_msg fail", __func__ );
		if( m_udpType == UDP_CLIENT ) {
			m_socket.async_receive(
					boost::asio::buffer(m_msg.data(), sizeof(cpMsg_t)),
					m_strand.wrap(
						boost::bind(&UdpSession::handleRead, shared_from_this(),
							boost::asio::placeholders::error)) ) ;
		} else 
			m_socket.async_receive_from(
					boost::asio::buffer(m_msg.data(), sizeof(cpMsg_t)), m_epRemote,
					m_strand.wrap(
						boost::bind(
							&UdpSession::handleRead, shared_from_this(),
							boost::asio::placeholders::error)) );
	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}

void UdpSession::handleWrite(const boost::system::error_code& error)
{
	if (!error) {

	} else {
		m_handler->error( error, shared_from_this(), m_owner );
	}
}


