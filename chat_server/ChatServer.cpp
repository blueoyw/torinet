#include "ChatServer.h"

ChatServer::ChatServer(const string name, const ServerConfig& config )
		: server_(new TcpServer(name, config))
          , ios_(server_->getIoServicePool()->getIoService() )
          , redis_(new Redis())
          //, m_signals(ios_), m_pIoPool(pool)
{
    rooms_.clear();
    characters_.clear();
}

void ChatServer::start(string ip, int port)
{
    LOG(L_INF, "[%s] ip[%s] port[%d]", __func__, ip.c_str(), port);

    if (redis_->connect(ip) == false)
    {
        LOG(L_ERR, "[%s] ip[%s] redis connect fail", __func__, ip.c_str());
        return;
    }

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
        [this](const Ptr<Session>& session)
        {
            messageHandler(session);
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

void ChatServer::messageHandler(const Ptr<Session>& session)
{
    Msg& msg = session->getMsg();
    int id = msg.head()->msgId;
    LOG(L_INF, "[%s] SESSION ID[%d] data[%p] size[%d]", __func__, session->getID(), msg.body(), msg.bodyLength());

    //handle messge
    switch(id)
    {
    case RQ_CREATE_USER:
        rqCreateUser(session);
        break;
    case NF_MESSAGE:
        nfMessage(session);
        break;
    default:
        LOG (L_ERR, "[%s] unknown id[%d]", __func__, id);
        break;
    }

}

void ChatServer::rqCreateUser(const Ptr<Session>& session)
{
    LOG (L_DEB, "[%s] start", __func__);
    RqCreateUser* rqCreateUser = (RqCreateUser*)session->getMsg().body();
    LOG(L_INF,"[%s] session id[%d] id[%s]", __func__, session->getID(), rqCreateUser->id);
    string id = rqCreateUser->id;
    string cmd = "zadd user:ranking 0 "+to_string(session->getID());
    LOG(L_INF,"[%s] cmd[%s]", __func__, cmd.c_str());

    redis_->send(cmd);
}

void ChatServer::nfMessage(const Ptr<Session>& session)
{
    LOG (L_DEB, "[%s] start", __func__);
    NfMessage* msg = (NfMessage*)session->getMsg().body();
    int length = strlen(msg->message);
    LOG(L_INF,"[%s] message[%s] length[%d]", __func__, msg->message, length);
    string cmd = "zincrby rank "+to_string(length)+" "+to_string(session->getID());

    redis_->send(cmd);
    {
        std::lock_guard<std::mutex> guard(server_->_mutex);
        auto it = server_->_sessions.begin();
        for ( ; it != server_->_sessions.end(); it++)
        {
            if (session == it->second)
                continue;
            it->second->send(session->getMsg());
        }
    }
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

