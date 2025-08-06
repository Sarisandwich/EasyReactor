#pragma once

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"

class TcpServer
{
private:
    EventLoop loop_;    //一个TcpServer可以有多个事件循环。目前单线程只启用一个。
public:
    TcpServer(const std::string& ip, uint16_t port);    //构造函数。传入ip和端口。
    ~TcpServer();   //析构函数。

    void start();   //运行事件循环。
};