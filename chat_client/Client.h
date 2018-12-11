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

	Client( const string& host, const int port)
	{
		m_pid = getpid();

        /*
		m_signals.add(SIGINT);
		m_signals.add(SIGTERM);
		m_signals.async_wait(boost::bind(&Client::stop, this) );
        */
        ClientConfig config;
		boost::asio::ip::tcp::endpoint ep(
				boost::asio::ip::address::from_string(host), port);
        UniquePtr<tcp::socket> socket(new tcp::socket(m_ios));
        m_session = Ptr<TcpSession>(new TcpSession( *socket, config, ep));
	}

    void registerHandler();

    void start()
    {
        m_session->connect();
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

private:
    Ptr<TcpSession> m_session;
	boost::asio::io_service     m_ios;
	int							m_pid;
};
