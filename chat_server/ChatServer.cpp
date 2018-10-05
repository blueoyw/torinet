#include "ChatServer.h"

ChatServer::ChatServer(const string name, const ServerConfig& config )
		: server_(new TcpServer(name, config))
          , ios_(server_->getIoServicePool()->getIoService() )
          //, m_signals(ios_), m_pIoPool(pool)
{
    rooms_.clear();
    characters_.clear();
}

void ChatServer::start(string ip, int port)
{
    LOG(L_INF, "[%s] ip[%s] port[%d]", __func__, ip.c_str(), port);

    //register handler
    server_->registerSessionOpenedHandler(
        [this](const Ptr<Session>& session)
        {
            openedHandler(session);
        });
    server_->registerSessionClosedHandler(
        [this](const Ptr<Session>& session, const CloseReason& reason)
        {
            closedHandler(session, reason);
        });
    server_->registerMessageHandler(
        [this](const Ptr<Session>& session, const uint8_t* data, size_t size)
        {
            messageHandler(session, data, size);
        });

    server_->start(ip, port);
    
}

void ChatServer::stop(const boost::system::error_code& error, int sigNum)
{
    LOG(L_INF, "[%s] error[%s] sigNum[%d]", __func__, error.message().c_str(), sigNum);
    /*
    LOG(L_FAT, "[%s] sigNum[%d]", __func__, sigNum);
    if(error) {
        LOG(L_FAT, "[%s] error[%s]", __func__, error.message().c_str() );
        return;
    }

    server_->handleStop();
    if( m_ioMode == MULTI_ASIO ) {
        m_pIoPool->stop();
        return;
    }
    */
    ios_.stop();
}

void ChatServer::openedHandler(const Ptr<Session>& session)
{
    LOG(L_INF, "[%s] ID[%d]", __func__, session->getID());
    //Msg를 받아서 처리.
}

void ChatServer::closedHandler(const Ptr<Session>& session, const CloseReason& reason)
{
    LOG(L_INF, "[%s] ID[%d] reason[%d]", __func__, session->getID(), reason);
}

void ChatServer::messageHandler(const Ptr<Session>& session, const uint8_t* data, size_t size)
{
    LOG(L_INF, "[%s] ID[%d] data[%p] size[%ld]", __func__, session->getID(), data, size);

}

void ChatServer::onMessage()
{

}

Ptr<Character> ChatServer::getCharacter(int id )
{
    LOG(L_INF, "[%s] id[%d]", __func__, id);
    auto tmp = characters_.find(id);
    if (tmp == characters_.end())
    {
        LOG(L_ERR, "[%s] not found[%d]", __func__, id);
        return nullptr;
    }
    return tmp->second;
}

