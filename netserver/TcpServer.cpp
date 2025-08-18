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
    printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    conn->set_closecb(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn->set_errorcb(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn->set_onmessagecb(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->set_sendCompletecb(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));

    conns_[conn->fd()]=conn;
}

void TcpServer::closeConnection(Connection* conn)
{
    printf("client(fd=%d) closed connection.\n", conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorConnection(Connection* conn)
{
    printf("client(fd=%d) error.\n", conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection* conn, std::string message)
{
    //对message进行某些处理。
    printf("message(eventfd=%d): %s\n", conn->fd(), message.c_str());

    message="reply:"+message;

    int len=message.size();
    std::string tmpbuf((char*)&len, 4);
    tmpbuf.append(message);

    conn->send(tmpbuf.data(), len+4);
}

void TcpServer::sendComplete(Connection* conn)
{
    printf("send complete.\n");

    /////////////////////
    //根据业务需求拓展代码。
    /////////////////////
}

void TcpServer::epollTimeout(EventLoop* loop)
{
    printf("epoll_wait() timeout.\n");

    /////////////////////
    //根据业务需求拓展代码。
    /////////////////////
}