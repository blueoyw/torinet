#ifndef _IPC_SERVER_H_
#define _IPC_SERVER_H_
#include "Incl.h"
#include "IpcSession.h"

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using namespace std;


class IpcServer
:public boost::enable_shared_from_this<IpcServer>
{
public:
	IpcServer(boost::asio::io_service& io_service, const std::string& file,
			SessionHandlerPtr handler )
		: m_ios(io_service),
		m_acceptor(io_service, stream_protocol::endpoint(file)),
		m_handler(handler)
	{

	}

	boost::shared_ptr< IpcServer > this_obj(){ return shared_from_this(); }

	void start_accept()
	{
		m_newSession.reset(new IpcSession(m_ios, m_handler));

		m_acceptor.async_accept(m_newSession->socket(),
				boost::bind(&IpcServer::handle_accept, shared_from_this(), m_newSession,
					boost::asio::placeholders::error));
	}

	void handle_accept(IpcSession_ptr session,
			const boost::system::error_code& error)
	{
		string tid = boost::lexical_cast<std::string>(boost::this_thread::get_id());
		LOG(L_INF, "[%s] thread id[%s]", __func__, tid.c_str());
		if (!error) {
			session->set_owner( this_obj() );
			session->start();
			session->async_read();
		}

		start_accept();
	}

private:
	boost::asio::io_service& 	m_ios;
	stream_protocol::acceptor 	m_acceptor;
	SessionHandlerPtr				m_handler;
	IpcSessionPtr					m_newSession;
};

typedef boost::shared_ptr<IpcServer> IpcServerPtr;
typedef boost::weak_ptr<IpcServer> IpcServerWeakPtr;


#endif	

