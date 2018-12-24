#include "Client.h"
Client::Client( const string& host, const int port)
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
    m_session = Ptr<TcpSession>(new TcpSession( std::move(socket), config, ep));
}

void Client::registerHandler()
{
    SLOG;
    m_session->_openedHandler = [this](const Ptr<Session>& session)
        {
            openedHandler(session);
        };
    m_session->_closedHandler = [this](const Ptr<Session>& session, const CloseReason& reason)
        {
            closedHandler(session, reason);
        };
    m_session->_messageHandler = [this](const Ptr<Session>& session)
        {
            messageHandler(session);
        };
}


void Client::openedHandler(const Ptr<Session>& session)
{
    LOG(L_INF, "[%s] ID[%d]", __func__, session->getID());
}

void Client::closedHandler(const Ptr<Session>& session, const CloseReason& reason)
{
    LOG(L_INF, "[%s] ID[%d] reason[%d]", __func__, session->getID(), reason);

}

void Client::messageHandler(const Ptr<Session>& session)
{
    Msg& msg = session->getMsg();
    int id = session->getID();
    LOG(L_INF, "[%s] ID[%d] data[%p] size[%d]", __func__, id, msg.body(), msg.bodyLength());

    switch(id)
    {
    case RQ_CREATE_USER:
        break;
    case NF_MESSAGE:
        break;
    default:
        LOG (L_ERR, "[%s] unknown id[%d]", __func__, id);
        break;
    }
}
