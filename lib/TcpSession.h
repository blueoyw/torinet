#pragma once
#include "Incl.h"
#include "Session.h"


namespace tori{
namespace net{

using tcp = boost::asio::ip::tcp;
using deadline_timer = boost::asio::deadline_timer;
using namespace std;

class TcpSession
:public Session, public boost::enable_shared_from_this<TcpSession>
{
public:

	TcpSession(const TcpSession&) = delete;
	TcpSession& operator=(const TcpSession&) = delete;
	TcpSession( asio::io_service& ios, UniquePtr<tcp::socket> socket, int id, ServerConfig& config );

	virtual ~TcpSession();

	virtual int getID() const
	{
		return _id;
	}

	virtual bool getRemoteEndpoint( string& ip, uint16_t& port ) const ;
	virtual void send( const uint8_t* data, size_t size ) ;
	virtual void send ( const Msg& msg ) ;
	virtual void send( Msg& msg ) ;
	virtual void start() ;
	virtual void close() ;
	virtual bool isOpen() const ;
	void 	handleWrite(const error_code& error);


#if 0
	asio::strand getStrand() 
	{
		return *_strand;
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
#endif
private:
	enum State
	{
		Ready,
		Opening,
		Opened,
		Closed
	};

	UniquePtr<tcp::socket> 	_socket;			
	int 	_id;
	State 		_state;
	tcp::endpoint  			_remoteEndpoint;
	//asio::io_service::strand _strand;
	IoMode 				_ioMode;

	Msg 				_msg;
	Msg 				_sendMsg;
    deque<Msg>        	_sendQue;
    uint64_t                _tx;
    uint64_t                _rx;

};

}
}

