#include"TcpServer.h"


TcpServer::TcpServer(const std::string& ip, uint16_t port)
{
    acceptor_=new Acceptor(&loop_, ip, port);
    acceptor_->set_newConnection_cb(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
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