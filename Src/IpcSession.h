#ifndef _IPC_SESSION_H_
#define _IPC_SESSION_H_
#include "Incl.h"
#include "IfSession.h"

using boost::asio::local::stream_protocol;
using boost::asio::deadline_timer;
using namespace std;

class IpcSession
:public boost::enable_shared_from_this<IpcSession>
{
public:
	//for client
	IpcSession(boost::asio::io_service& ios, const string& file,
			SessionHandlerPtr handler )
		: m_socket(ios), m_msgTmr(ios), m_epRemote(stream_protocol::endpoint(file)),
		  m_handler(handler),
		  m_strand(ios)
	{

	}

	//for server
	IpcSession(boost::asio::io_service& ios, SessionHandlerPtr handler)
		: m_socket(ios), m_msgTmr(ios), 
		m_handler(handler),
		m_strand(ios)
	{

	}

	stream_protocol::socket& socket()
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

	void 	asyncConnect();
	void 	asyncRead();
	void 	asyncSend();

	void 	handleConnect(const boost::system::error_code& error);
	void 	handleReadHeader(const boost::system::error_code& error);
	void 	handleReadBody(uint32_t& msgId, int16_t& length, const boost::system::error_code& error);
	void 	handleWrite(const boost::system::error_code& error);

	deadline_timer&	msgTmr() { return m_msgTmr; }
	Msg& 					msg() { return m_msg; }
	Msg& 					sendMsg() { return m_sendMsg; }
protected:
	stream_protocol::socket 	m_socket;			
	Msg 				m_msg;
	Msg 				m_sendMsg;
	deadline_timer	m_msgTmr;		
	stream_protocol::endpoint	m_epRemote;

private:
	SessionHandlerPtr				m_handler;

	boost::asio::io_service::strand m_strand;
	VoidWeakPtr				m_owner;

};

typedef boost::shared_ptr<IpcSession> IpcSessionPtr;
#endif		//_SESSION_H_

