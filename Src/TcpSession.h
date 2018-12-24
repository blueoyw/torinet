#pragma once
#include "Incl.h"
#include "Session.h"


namespace tori{
namespace net{

using tcp = boost::asio::ip::tcp;
using deadline_timer = boost::asio::deadline_timer;
using namespace std;

class TcpSession
:public Session, public std::enable_shared_from_this<TcpSession>
{
public:

	TcpSession(const TcpSession&) = delete;
	TcpSession& operator=(const TcpSession&) = delete;
	TcpSession( UniquePtr<tcp::socket> socket, int id, ServerConfig& config );
	TcpSession( UniquePtr<tcp::socket> socket, ClientConfig& config, tcp::endpoint& ep);

	virtual ~TcpSession() {}

	virtual int getID() const
	{
		return _id;
	}

	virtual bool getRemoteEndpoint( string& ip, uint16_t& port ) const ;
	virtual void send( const uint8_t* data, size_t size ) ;
	virtual void send ( const Msg& msg ) ;
	virtual void send( Msg& msg ) ;
	virtual void read( size_t size ) ;
	virtual void start() ;
    virtual void connect();
	virtual void close() ;
	virtual bool isOpen() const ;

    virtual Msg& getMsg() { return _msg; }

    std::function<void(const Ptr<TcpSession>&)> _openedHandler;
    std::function<void(const Ptr<TcpSession>&, const CloseReason&)>  _closedHandler;
    std::function<void(const Ptr<TcpSession>&)> _messageHandler;

private:
	void 	handleWrite( const error_code& error);
	void 	handleReadHeader( const error_code& ec );
	void 	handleReadBody( uint32_t& msgId, int16_t& length,  const boost::system::error_code& ec);
	void 	handleError( const error_code& ec);

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
    deque<Msg>        	_sendQue;

	size_t _minReceive;
	size_t _maxReceiveBufferSize;

    uint64_t                _tx;
    uint64_t                _rx;

};

}
}

