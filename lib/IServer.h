#pragma once
#include "Incl.h"
#include "IoServicePool.h"
#include "ISession.h"

class IServer
{
public:
	enum class State { Ready=0, Start, Stop };
	using ConnectHandler = std::function<void( ISession& session )>;
	using ReadHandler = std::function<void( ISession& session, uint8_t* buf, size_t size )>;
	using ErrorHandler = std::function<void( ISession& session, const error_code& ec )>;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	virtual ~Server(){}

	virtual void start(uint16_t port) = 0;
	virtual void stop() = 0;

	virtual State getState() = 0;
	virtual Ptr<IoServicePool> getIoServicePool() = 0;
	
	virtual void registerConnectHandler( ConnectHandler handler ) = 0;
	virtual void registerReadHandler( ReadHandler handler ) = 0;
	virtual void registerErrorHandler( ErrorHandler handler ) = 0;
};

