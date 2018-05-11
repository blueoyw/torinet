#pragma once
#include "Incl.h"
#include "ISession.h"

class TcpSession
:public ISession, public std::enable_shared_from_this<TcpSession>
{
public:
	using tcp = boost::asio::ip::tcp;
	//using deadline_timer = boost::asio::deadline_timer;

	TcpSession(const TcpSession&) = delete;
	TcpSession& operator=(const TcpSession&) = delete;
	TcpSession(long& id, std::unique_ptr<tcp::socket> socket, const IoMode& ioMode );


	virtual long getID() override{ return _id; }
	virtual State getState() override { return _state; }
	virtual bool getRemoteEndpoint( std::string& ip, uint16_t& port ) override ;
	virtual void send( const uint8_t* data, size_t size ) override;
	virtual void send( const Msg& buf ) override;
	virtual void read() override;
	virtual void start() override;
	virtual void close() override;
	virtual bool isOpen() override;
	//virtual asio::strand& getStrand() override;

	virtual void registerConnectHandler( ConnectHandler& handler ) override
	{
		_connectHandler = handler;
	}

	virtual void registerReadHandler( ReadHandler& handler ) override
	{
		_readHandler = handler;
	}

	virtual void registerErrorHandler( ErrorHandler& handler ) override
	{
		_errorHandler = handler;
	}

	/*
	void setOwner( const VoidPtr& owner)
	{
		_owner = owner;
	}

	void 	asyncConnect();
	void 	asyncRead();
	void 	asyncReadOne();
	void 	asyncSend();
	void	asyncSend( Msg& msg );

	void 	handleConnect(const boost::system::error_code& error);
	void 	handleReadHeader(const boost::system::error_code& error);
	void 	handleReadBody(uint32_t& msgId, int16_t& length, const boost::system::error_code& error);
	void 	handleWrite(const boost::system::error_code& error);
	*/

	//void broadcast(const Msg& str);

	//getter
	//deadline_timer&	msgTmr() { return _msgTmr; }
	Msg& 					msg() { return _msg; }
	Msg& 					sendMsg() { return _sendMsg; }
	/*
	void 					encodeSendMsg(const MsgId_t& msgId, const unsigned char* data, const int& dataLength)
	{
		if( _ioMode == MULTI_ASIO ) {
			boost::bind(&Msg::encodeMsg, &_sendMsg,
						msgId, data, dataLength);
			return;
		} else
			_strand.dispatch( boost::bind(&Msg::encodeMsg, &_sendMsg,
						msgId, data, dataLength) );
	}
	*/

    uint64_t tx() { return _tx; }
    uint64_t rx() { return _rx; }

private:
	long				_id;
	std::unique_ptr<tcp::socket> _socket;			
	Msg 				_msg;
	Msg 				_sendMsg;
	//deadline_timer		_msgTmr;		
	tcp::endpoint		_remoteEndpoint;

	IoMode 				_ioMode;

	//boost::asio::io_service::strand _strand;
	std::unique_ptr<asio::io_service::strand> _strand;
	//std::unique_ptr<asio::strand> _strand;
	//VoidWeakPtr		_owner;

    MessageQueue        	_sendQue;
    uint64_t                _tx;
    uint64_t                _rx;
	State 					_state;
	ConnectHandler 			_connectHandler;
	ReadHandler 			_readHandler;
	ErrorHandler 			_errorHandler;
};
