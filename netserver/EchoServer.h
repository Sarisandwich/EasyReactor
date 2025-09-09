#pragma once
#include"TcpServer.h"
#include"EventLoop.h"
#include"Connection.h"
#include<syscall.h>
#include<unistd.h>

class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool pool_;

public:
    EchoServer(const std::string &ip,const uint16_t port, size_t workNumThread=5, size_t subNumThread=3);
    ~EchoServer();

    void Start();   // 启动服务。
    void Stop();    //停止服务。

    void HandleNewConnection(spConnection conn); // 处理新客户端连接请求，供TcpServer回调。
    void HandleClose(spConnection conn); // 关闭客户端的连接，供TcpServer回调。 
    void HandleError(spConnection conn); // 客户端的连接错误，供TcpServer回调。
    void HandleMessage(spConnection conn,std::string& message);   // 处理客户端的请求报文，供TcpServer回调。
    void HandleSendComplete(spConnection conn);  // 数据发送完成后，供TcpServer回调。
    // void HandleTimeOut(EventLoop *loop); // epoll_wait()超时，供TcpServer回调。
    void OnMessage(spConnection conn,std::string& message);  //处理客户端的请求报文。用于传递任务给线程池。
};