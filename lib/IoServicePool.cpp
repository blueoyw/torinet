#include "IoServicePool.h"

IoServicePool::IoServicePool(size_t size, IoMode& mode ):_next(0),_mode(mode)
{
	if( mode == MULTI ) {
		for( size_t i=0; i<size; i++) {
			std::unique_ptr<asio::io_service> ios(new asio::io_service );
			std::unique_ptr<asio::io_service::work>	work(new asio::io_service::work(*ios) );

			_ios.emplace_back(std::move(ios));
			_work.emplace_back(std::move(work));
		}
	}
	else {
		UniquePtr<asio::io_service> ios( new asio::io_service);
		UniquePtr<asio::io_service::work> work(new asio::io_service::work(*ios) );
		_work.emplace_back( std::move(work) );
		_ios.emplace_back( std::move(ios) );
	}
}

IoServicePool::~IoServicePool()
{
	stop();
	wait();
}

asio::io_service& IoServicePool::getIoService( )
{
	//round robin
	if( _next >= _ios.size() ) 
		_next = 0;

	return *_ios[_next++];
}

void IoServicePool::run()
{
	for( size_t i=0; i<_ios.size(); i++) {
		//Ptr<std::thread> thread( new std::thread(
			//boost::bind( &boost::asio::io_service::run, m_ios[i] ) ));
		std::thread thread = std::thread( [this, ios=_ios[i].get()]()
					{
						try
						{
							ios->run();
						}
						catch( const std::exception& e ) 
						{
							std::cout<<"io service exception="<< e.what();
						}

					} ) ;
		_threads.emplace_back( std::move(thread ) );

	}
}


void IoServicePool::wait()
{
	for( auto& thread : _threads) {
		if( thread.joinable() ) 
			thread.join();
	}
}

void IoServicePool::stop()
{
	//for( size_t i=0; i<_ios.size(); i++) {
	for( auto& ios : _ios ) {
		ios->stop();
	}
}
