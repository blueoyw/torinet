#ifndef _SESSION_H_
#define _SESSION_H_
#include "Incl.h"

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using namespace std;

class Server;
typedef boost::shared_ptr<Server> Server_ptr;
typedef boost::weak_ptr<Server> Server_weak_ptr;

typedef bool (*process_msg)( uint32_t& msgId, int16_t& length, void* sess );

class Session
:public boost::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_service& io_service)
		: socket_(io_service), msgTmr_(io_service), cb_(NULL), strand_(io_service)
	{

	}

	Session(boost::asio::io_service& io_service, process_msg cb)
		: socket_(io_service), msgTmr_(io_service), cb_(cb), strand_(io_service)
	{

	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void set_server( const unsigned int& idx, const Server_ptr& server)
	//void set_server( const unsigned int& idx, const Server_ptr server)
	{
		idx_	=	idx;
		server_ = server;
	}

	void	start();
	void	open();
	void	close();

	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(uint32_t& msgId, int16_t& length, const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error);

	void send();
	void delete_session();

	unsigned int idx() { return idx_; }

	deadline_timer&	        msgTmr() { return msgTmr_; }
	Msg& 					msg() { return msg_; }
protected:
	tcp::socket 	socket_;			
	Msg 			msg_;
	MessageQueue 	m_sendQue;
	deadline_timer	msgTmr_;		
	process_msg		cb_;

private:
	boost::asio::io_service::strand strand_;
	Server_weak_ptr			server_;
	unsigned int				idx_;
};

typedef boost::shared_ptr<Session> Session_ptr;
#endif		//_SESSION_H_

