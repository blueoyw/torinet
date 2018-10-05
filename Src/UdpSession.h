#ifndef _UDP_SESSION_H_
#define _UDP_SESSION_H_
#include "Incl.h"
#include "IfSession.h"

using boost::asio::ip::udp;
using boost::asio::deadline_timer;
using namespace std;

typedef enum {
	UDP_CLIENT = 0,
	UDP_SERVER 
} UdpType;

class UdpSession
:public boost::enable_shared_from_this<UdpSession>
{
public:
	//for client
	UdpSession(boost::asio::io_service& ios, const udp::endpoint& local
			, const udp::endpoint& remote,
			SessionHandlerPtr handler)
		: m_socket(ios), m_epLocal(local), m_epRemote(remote), 
		m_udpType(UDP_CLIENT), m_msgTmr(ios), 
		m_handler(handler),
		m_strand(ios)
	{

	}

	//for server
	UdpSession(boost::asio::io_service& ios, const udp::endpoint& local, 
			SessionHandlerPtr handler)
		: m_socket(ios, local), m_epLocal(local), m_udpType(UDP_SERVER),
		m_msgTmr(ios), m_handler(handler),
		m_strand(ios)
	{

	}

	udp::socket& socket()
	{
		return m_socket;
	}

	void setOwner( const VoidPtr& owner)
	{
		m_owner = owner;
	}

	void	start();
	void	open();
	void	close();

	void	asyncConnect();
	void	asyncRead();
	void	asyncSend();

	void	handleConnect(const boost::system::error_code& error);
	void	handleRead(const boost::system::error_code& error);
	void	handleWrite(const boost::system::error_code& error);

	//void broadcast(const Msg& str);

	deadline_timer&	msgTmr() { return m_msgTmr; }
	Msg& 					msg() { return m_msg; }
	Msg& 					sendMsg() { return m_sendMsg; }
	UdpType&			udpType() { return m_udpType; }
protected:
	udp::socket 	m_socket;			
	udp::endpoint	m_epLocal;
	udp::endpoint	m_epRemote;
	UdpType			m_udpType;

	Msg 				m_msg;
	Msg 				m_sendMsg;
	deadline_timer	m_msgTmr;		

private:
	SessionHandlerPtr						m_handler;
	boost::asio::io_service::strand	m_strand;
	VoidWeakPtr           m_owner;
};

typedef boost::shared_ptr<UdpSession> UdpSessionPtr;
#endif		//_SESSION_H_

