#pragma once

#include "Incl.h"

using namespace boost::asio;
using namespace std;
using namespace tori;
using namespace tori::net;

extern bool g_bTerm;
class Client
:public boost::enable_shared_from_this<Client>
{
public:
	static const int MASTER_CLIENT = 0;

	Client( const string& host, const int port);

    void registerHandler();

    void start()
    {
        SLOG;
        registerHandler();
        m_session->connect();
		LOG(L_FAT,"[%s] %d client start", __func__, m_pid);
        m_ios.run();
    }

	void stop()
	{
		LOG(L_FAT,"[%s] %d client stop", __func__, m_pid);
		m_ios.stop();	
	}

	void close()
	{
		// Cancel all outstanding asynchronous operations.
		m_session->close();
	}

	void send(Msg& msg)
	{
		//m_ios.post( boost::bind(&Client::doWrite, shared_from_this(), msg));
        m_session->send(msg);
	}

    //handlers
    void openedHandler(const Ptr<Session>& session);
    void closedHandler(const Ptr<Session>& session, const CloseReason& reason);
    void messageHandler(const Ptr<Session>& session);

    Ptr<TcpSession> m_session;
private:
	boost::asio::io_service     m_ios;
	int							m_pid;
};
