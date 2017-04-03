#ifndef _TCP_SESSION_H_
#define _TCP_SESSION_H_
#include "Incl.h"
#include "IfSession.h"

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using namespace std;

class TcpSession
:public boost::enable_shared_from_this<TcpSession>
{
public:
	//for client
	TcpSession(boost::asio::io_service& ios, const tcp::endpoint& epRemote,
			SessionHandlerPtr handler )
		: m_socket(ios), m_msgTmr(ios), m_epRemote(epRemote),
		  m_handler(handler),
		  m_ioMode(SINGLE_ASIO), m_strand(ios), m_tx(0), m_rx(0)
	{

	}

	//for server
	TcpSession(boost::asio::io_service& ios, const IoMode& ioMode, SessionHandlerPtr handler)
		: m_socket(ios), m_msgTmr(ios), 
		m_handler(handler),
		m_ioMode(ioMode), m_strand(ios), m_tx(0), m_rx(0)
	{

	}

	tcp::socket& socket()
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
	void 	asyncReadOne();
	void 	asyncSend();
	void	asyncSend( Msg& msg );

	void 	handleConnect(const boost::system::error_code& error);
	void 	handleReadHeader(const boost::system::error_code& error);
	void 	handleReadBody(uint32_t& msgId, int16_t& length, const boost::system::error_code& error);
	void 	handleWrite(const boost::system::error_code& error);

	//void broadcast(const Msg& str);

	//getter
	deadline_timer&	msgTmr() { return m_msgTmr; }
	Msg& 					msg() { return m_msg; }
	Msg& 					sendMsg() { return m_sendMsg; }
	void 					encodeSendMsg(const MsgId_t& msgId, const unsigned char* data, const int& dataLength)
	{
		if( m_ioMode == MULTI_ASIO ) {
			boost::bind(&Msg::encodeMsg, &m_sendMsg,
						msgId, data, dataLength);
			return;
		} else
			m_strand.dispatch( boost::bind(&Msg::encodeMsg, &m_sendMsg,
						msgId, data, dataLength) );
	}
	string&	id() { return m_id; }
    uint64_t tx() { return m_tx; }
    uint64_t rx() { return m_rx; }

	//setter
	bool		setId(const string& id) { if(!m_id.empty()) return false; m_id=id; return true; }

protected:
	tcp::socket 	m_socket;			
	Msg 				m_msg;
	Msg 				m_sendMsg;
	deadline_timer	m_msgTmr;		
	tcp::endpoint	m_epRemote;

private:
	SessionHandlerPtr	m_handler;
	IoMode 				m_ioMode;

	boost::asio::io_service::strand m_strand;
	VoidWeakPtr		m_owner;
	string			m_id;

    MessageQueue        m_sendQue;
    uint64_t                m_tx;
    uint64_t                m_rx;
};

typedef boost::shared_ptr<TcpSession> TcpSessionPtr;
#endif		//_SESSION_H_

