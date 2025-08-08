#pragma once

#include<functional>

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"InetAddress.h"

//封装客户端通讯的Channel。
class Connection
{
private:
    EventLoop* loop_;   //Connection对应的事件循环。外部传入。
    Socket* clientsock_;  //与客户端通讯的socket。外部传入。虽然是外部传入，但逻辑上仍属于类内成员，需要由Connection析构释放。
    Channel* clientchannel_;    //Connection对应的channel。构造函数创建。类内成员。
public:
    Connection(EventLoop* loop, Socket* clientsock);  //构造函数。
    ~Connection();    //析构函数。

    int fd() const; //返回fd。
    std::string ip() const; //返回ip。
    uint16_t port() const;  //返回端口。
};