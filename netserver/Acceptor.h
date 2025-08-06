#pragma once

#include<functional>

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"InetAddress.h"

//封装监听连接的Channel。
class Acceptor
{
private:
    EventLoop* loop_;   //Acceptor对应的事件循环。外部传入。
    Socket* servsock_;  //服务端用于监听的socket。构造函数创建。类内成员。
    Channel* acceptchannel_;    //Acceptor对应的channel。构造函数创建。类内成员。
public:
    Acceptor(EventLoop* loop, const std::string& ip, uint16_t port);  //构造函数。
    ~Acceptor();    //析构函数。
};