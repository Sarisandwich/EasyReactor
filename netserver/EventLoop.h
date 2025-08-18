#pragma once

#include"Epoll.h"

class Channel;
class Epoll;

//事件循环类。
class EventLoop
{
private:
    Epoll* ep_; //每个事件循环只对应一个ep。
    std::function<void(EventLoop*)> epollTimeout_cb_;   //回调函数。epoll_wait()超时的处理。
public:
    EventLoop();    //构造函数。创建ep。
    ~EventLoop();    //析构函数。销毁ep。

    void run(); //运行事件循环。

    void update_channel(Channel* ch);   //加入或修改红黑树内容。

    void set_epollTimeoutcb(std::function<void(EventLoop*)>);   //设置epollTimeout_cb_。
};