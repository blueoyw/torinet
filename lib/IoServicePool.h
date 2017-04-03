#ifndef _IO_SERVICE_POOL_
#define _IO_SERVICE_POOL_
#include "Incl.h"

typedef boost::shared_ptr<boost::asio::io_service> IosPtr;
typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

class IoServicePool
	: private boost::noncopyable
{
public:
	//enum { MAX_POOL = 32 };
	explicit IoServicePool(size_t size);
	//IoServicePool(size_t size);

	boost::asio::io_service& getIoService();

	void run();

	void stop();

private:
	std::vector<IosPtr>				m_ios;
	std::vector<WorkPtr>				m_work;
	size_t								m_next;
};

typedef boost::shared_ptr<IoServicePool> IoServicePoolPtr;

#endif
