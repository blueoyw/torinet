#pragma once
#include "Incl.h"

class ISession
{
public:
	enum class State
	{
		Ready,
		Opening,
		Opened,
		Closed
	};

	using ConnectHandler = std::function<void( ISession& session )>;
	using ReadHandler = std::function<void( ISession& session, uint8_t* buf, size_t size )>;
	using ErrorHandler = std::function<void( ISession& session, const error_code& ec )>;

	ISession(const ISession&) = delete;
	ISession& operator=(const ISession&) = delete;
	virtual ~ISession(){}

	virtual long getID() = 0;
	virtual State getState() = 0;
	virtual bool getRemoteEndpoint( std::string& ip, uint16_t& port ) = 0;
	virtual void send( const uint8_t* data, size_t size ) = 0;
	virtual void send( const Msg& buf ) = 0;
	virtual void read() = 0;
	virtual void start() = 0;
	virtual void close() = 0;
	virtual bool isOpen() = 0;
	//virtual asio::strand& getStrand() = 0;

	virtual void registerConnectHandler( ConnectHandler& handler ) = 0;
	virtual void registerReadHandler( ReadHandler& handler ) = 0;
	virtual void registerErrorHandler( ErrorHandler& handler ) = 0;
protected:
	ISession() {}
};
