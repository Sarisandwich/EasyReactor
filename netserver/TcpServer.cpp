#include"TcpServer.h"


TcpServer::TcpServer(const std::string& ip, uint16_t port)
{
    acceptor_=new Acceptor(&loop_, ip, port);
    acceptor_->set_newConnection_cb(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
    loop_.set_epollTimeoutcb(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    for(auto& conn:conns_)
    {
        delete conn.second;
    }
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newConnection(Socket* clientsock)
{
    Connection* conn=new Connection(&loop_, clientsock);
    //printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    conn->set_closecb(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn->set_errorcb(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn->set_onmessagecb(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->set_sendCompletecb(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));

    conns_[conn->fd()]=conn;

    if(newConnection_cb_) newConnection_cb_(conn);
}

void TcpServer::closeConnection(Connection* conn)
{
    if(closeConnection_cb_) closeConnection_cb_(conn);

    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorConnection(Connection* conn)
{
    if(errorConnection_cb_) errorConnection_cb_(conn);

    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection* conn, std::string& message)
{
    if(onmessage_cb_) onmessage_cb_(conn, message);
}

void TcpServer::sendComplete(Connection* conn)
{
    if(sendComplete_cb_) sendComplete_cb_(conn);
}

void TcpServer::epollTimeout(EventLoop* loop)
{
    if(epollTimeout_cb_) epollTimeout_cb_(loop);
}

void TcpServer::set_newConnectioncb(std::function<void(Connection*)> func)
{
    newConnection_cb_=func;
}

void TcpServer::set_closeConnectioncb(std::function<void(Connection*)> func)
{
    closeConnection_cb_=func;
}

void TcpServer::set_errorConnectioncb(std::function<void(Connection*)> func)
{
    errorConnection_cb_=func;
}

void TcpServer::set_onmessagecb(std::function<void(Connection*,std::string& message)> func)
{
    onmessage_cb_=func;
}

void TcpServer::set_sendCompletecb(std::function<void(Connection*)> func)
{
    sendComplete_cb_=func;
}

void TcpServer::set_epollTimeoutcb(std::function<void(EventLoop*)> func)
{
    epollTimeout_cb_=func;
}