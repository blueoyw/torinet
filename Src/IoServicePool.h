#pragma once
#include "Incl.h"

namespace tori{
namespace net {
typedef boost::shared_ptr<boost::asio::io_service> IosPtr;
typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

class IoServicePool
{
public:
	//enum { MAX_POOL = 32 };
	IoServicePool( const IoServicePool&) = delete;
	IoServicePool& operator=( const IoServicePool&) = delete;

	explicit IoServicePool(size_t size);
	//~IoServicePool();
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

}
}
