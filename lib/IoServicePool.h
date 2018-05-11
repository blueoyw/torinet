#pragma once
#include "Incl.h"
#include <thread>
#include <vector>

class IoServicePool
	: private boost::noncopyable
{
public:
	enum { MAX_POOL = 16 };

	IoServicePool( const IoServicePool&) =delete;
	IoServicePool& operator=( const IoServicePool&) =delete;

	explicit IoServicePool(size_t size, IoMode& mode );
	~IoServicePool();

	boost::asio::io_service& getIoService();

	void run();

	void stop();
	void wait();

private:
	std::vector<UniquePtr<asio::io_service>> _ios;
	std::vector<UniquePtr<asio::io_service::work>>	_work;
	std::vector< std::thread > _threads;
	size_t								_next;
	IoMode 	_mode;
};
