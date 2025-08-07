#pragma once

#include<functional>

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"InetAddress.h"
#include"Connection.h"

//封装监听连接的Channel。
class Acceptor
{
private:
    EventLoop* loop_;   //Acceptor对应的事件循环。外部传入。
    Socket* servsock_;  //服务端用于监听的socket。构造函数创建。类内成员。
    Channel* acceptchannel_;    //Acceptor对应的channel。构造函数创建。类内成员。
private:
    //回调函数。
    std::function<void(Socket*)> newConnection_cb_; //新建Connection对象的回调函数。
public:
    Acceptor(EventLoop* loop, const std::string& ip, uint16_t port);  //构造函数。
    ~Acceptor();    //析构函数。

    void newConnection();   //处理新客户端连接请求。
    void set_newConnection_cb(std::function<void(Socket*)> func);   //设置回调函数newConnection_cb_。
};