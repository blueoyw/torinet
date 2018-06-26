#pragma once
#include "Incl.h"
#include "Session.h"
#include "IoServicePool.h"


namespace tori{
namespace net{

using namespace std;

class Server
{
public:
	enum class State
	{
		Ready = 0,
		Start,
		Stop
	};

	/*
	// Create server instance.
	static Ptr<Server> create(const ServerConfig& config)
	{
		return make_shared<TcpServer>(config);
	}
	*/

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	virtual ~Server() {};

	virtual void start(uint16_t port) = 0;
	virtual void start(std::string address, uint16_t port) = 0;
	virtual void stop() = 0;

	virtual State getState() = 0;
	virtual Ptr<IoServicePool> getIoServicePool() = 0;
	virtual Ptr<Session> getSession(int id) = 0;

	virtual void registerSessionOpenedHandler(const SessionOpenedHandler& handler) = 0;
	virtual void registerSessionClosedHandler(const SessionClosedHandler& handler) = 0;
	virtual void registerMessageHandler(const MessageHandler& handler) = 0;

protected:
	Server() {};
};

} // namespace net
} // namespace gisun
