#pragma once
#include"TcpServer.h"
#include"EventLoop.h"
#include"Connection.h"

class EchoServer
{
private:
    TcpServer tcpserver_;

public:
    EchoServer(const std::string &ip,const uint16_t port);
    ~EchoServer();

    void Start();   // 启动服务。

    void HandleNewConnection(Connection *conn); // 处理新客户端连接请求，供TcpServer回调。
    void HandleClose(Connection *conn); // 关闭客户端的连接，供TcpServer回调。 
    void HandleError(Connection *conn); // 客户端的连接错误，供TcpServer回调。
    void HandleMessage(Connection *conn,std::string& message);   // 处理客户端的请求报文，供TcpServer回调。
    void HandleSendComplete(Connection *conn);  // 数据发送完成后，供TcpServer回调。
    // void HandleTimeOut(EventLoop *loop); // epoll_wait()超时，供TcpServer回调。
};