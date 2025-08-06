#pragma once

#include"Epoll.h"

//事件循环类。
class EventLoop
{
private:
    Epoll* ep_; //每个事件循环只对应一个ep。
public:
    EventLoop();    //构造函数。创建ep。
    ~EventLoop();    //析构函数。销毁ep。

    void run(); //运行事件循环。

    Epoll* ep();    //返回ep地址。
};