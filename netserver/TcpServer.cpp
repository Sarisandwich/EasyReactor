#include"TcpServer.h"


TcpServer::TcpServer(const std::string& ip, uint16_t port)
{
    acceptor_=new Acceptor(&loop_, ip, port);
    acceptor_->set_newConnection_cb(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newConnection(Socket* clientsock)
{
    Connection* conn=new Connection(&loop_, clientsock);
    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
}