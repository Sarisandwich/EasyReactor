#include"EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port, size_t workNumThread, size_t subNumThread)
                    :tcpserver_(ip, port, subNumThread), pool_(workNumThread, "WORKS")
{
    // 以下代码不是必须的，业务关心什么事件，就指定相应的回调函数。
    tcpserver_.set_newConnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.set_closeConnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.set_errorConnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.set_onmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.set_sendCompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.set_epollTimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{}

void EchoServer::Start()                
{
    tcpserver_.start();
}
#include<syscall.h>
#include<unistd.h>
// 处理新客户端连接请求，在TcpServer类中回调此函数。
void EchoServer::HandleNewConnection(spConnection conn)    
{
    
    // printf("HandleNewConnection thread(%ld).\n", syscall(SYS_gettid));

    std::cout << "New Connection Come in." << std::endl;

    ///////////////////////
    //根据业务需求，拓展代码。
    ///////////////////////
}

// 关闭客户端的连接，在TcpServer类中回调此函数。 
void EchoServer::HandleClose(spConnection conn)  
{
    std::cout << "EchoServer conn closed." << std::endl;

    ///////////////////////
    //根据业务需求，拓展代码。
    ///////////////////////
}

// 客户端的连接错误，在TcpServer类中回调此函数。
void EchoServer::HandleError(spConnection conn)  
{
    std::cout << "EchoServer conn error." << std::endl;

    ///////////////////////
    //根据业务需求，拓展代码。
    ///////////////////////
}

// 处理客户端的请求报文，在TcpServer类中回调此函数。
void EchoServer::HandleMessage(spConnection conn,std::string& message)     
{
    // printf("HandleMessage thread(%ld).\n", syscall(SYS_gettid));

    pool_.enqueue(std::bind(&EchoServer::OnMessage, this, conn, message));
}

void EchoServer::OnMessage(spConnection conn,std::string& message)
{
    /////////////////////////////
    // 在这里，将经过若干步骤的运算。
    /////////////////////////////
    message="reply: "+message;
    
    conn->send(message.data(),message.size());   // 把临时缓冲区中的数据发送出去。
}

// 数据发送完成后，在TcpServer类中回调此函数。
void EchoServer::HandleSendComplete(spConnection conn)     
{
    std::cout << "Message send complete." << std::endl;

    ///////////////////////
    //根据业务需求，拓展代码。
    ///////////////////////
}

/*
// epoll_wait()超时，在TcpServer类中回调此函数。
void EchoServer::HandleTimeOut(EventLoop *loop)         
{
    std::cout << "EchoServer timeout." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}
*/