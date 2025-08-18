#pragma once

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"Acceptor.h"
#include"Connection.h"

#include<map>


//TCP网络服务类。
class TcpServer
{
private:
    EventLoop loop_;    //一个TcpServer可以有多个事件循环。目前单线程只启用一个。
    Acceptor* acceptor_;    //一个TcpServer只有一个Acceptor对象。
    std::map<int,Connection*> conns_;   //map容器储存管理Connection对象。
private:
    //回调函数。
    std::function<void(Connection*)> newConnection_cb_; // 回调EchoServer::HandleNewConnection()。
    std::function<void(Connection*)> closeConnection_cb_;   // 回调EchoServer::HandleClose()。
    std::function<void(Connection*)> errorConnection_cb_;   // 回调EchoServer::HandleError()。
    std::function<void(Connection*,std::string &message)> onmessage_cb_;    // 回调EchoServer::HandleMessage()。
    std::function<void(Connection*)> sendComplete_cb_;  // 回调EchoServer::HandleSendComplete()。
    std::function<void(EventLoop*)>  epollTimeout_cb_;  // 回调EchoServer::HandleTimeOut()。
public:
    TcpServer(const std::string& ip, uint16_t port);    //构造函数。传入ip和端口。
    ~TcpServer();   //析构函数。

    void start();   //运行事件循环。

    void newConnection(Socket* clientsock);   //处理新客户端连接请求。
    void closeConnection(Connection* conn); //关闭客户端连接。供Connection回调。
    void errorConnection(Connection* conn); //客户端连接错误。供Connection回调。
    void onmessage(Connection* conn, std::string message);  //处理接收缓冲区的数据。供Connection回调。
    void sendComplete(Connection* conn);    //发送数据完成之后的操作。供Connection回调。
    void epollTimeout(EventLoop* loop);     //epollwait()超时之后的操作。供EventLoop回调。

    //设置回调函数。
    void set_newConnectioncb(std::function<void(Connection*)> func);
    void set_closeConnectioncb(std::function<void(Connection*)> func);
    void set_errorConnectioncb(std::function<void(Connection*)> func);
    void set_onmessagecb(std::function<void(Connection*,std::string &message)> func);
    void set_sendCompletecb(std::function<void(Connection*)> func);
    void set_epollTimeoutcb(std::function<void(EventLoop*)> func);
};