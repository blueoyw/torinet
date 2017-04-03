#include "IoServicePool.h"

IoServicePool::IoServicePool(size_t size):m_next(0)
{
	for( size_t i=0; i<size; i++) {
		IosPtr 	ios(new boost::asio::io_service );
		WorkPtr 				work(new boost::asio::io_service::work(*ios) );
		LOG(L_INF,"[%s] idx[%u] %p", __func__, i, ios.get() );

		m_ios.push_back(ios);
		m_work.push_back(work);
	}
}

//io_service& IoServicePool::getIoService( IoServicePoolPtr& ios )
boost::asio::io_service& IoServicePool::getIoService( )
{
	//round robin
	if( m_next >= m_ios.size() ) 
		m_next = 0;

	LOG(L_DEB,"[%s] idx[%u]", __func__, m_next);
	boost::asio::io_service& ios = *m_ios[m_next++];
	return ios;
}

void IoServicePool::run()
{
	LOG(L_INF,"[%s] pool num[%ld] ", __func__, m_ios.size() );
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for( size_t i=0; i<m_ios.size(); i++) {
		boost::shared_ptr<boost::thread> thread( new boost::thread(
			boost::bind( &boost::asio::io_service::run, m_ios[i] ) ));
		threads.push_back( thread );
	}

	for( size_t i=0; i<m_ios.size(); i++) {
		threads[i]->join();
	}
}

void IoServicePool::stop()
{
	for( size_t i=0; i<m_ios.size(); i++) {
		m_ios[i]->stop();
	}
}

