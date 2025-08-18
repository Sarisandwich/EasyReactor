#pragma once

#include<sys/epoll.h>
#include<functional>
#include"Epoll.h"
#include"Socket.h"
#include"EventLoop.h"

class EventLoop;

class Channel
{
private:
    int fd_=-1;
    EventLoop* loop_=nullptr; //Channel类需要外部传入fd和EventLoop，Channel类对它们没有修改权限，只是需要它们的信息。
    //Channel与fd是一对一，Channel与ep是多对一。
    //使用Channel类，ev.data.ptr指向Channel，取代ev.data.fd，这样可以携带更多信息。

    bool inepoll_=false;    //Channel是否已在红黑树上。
    uint32_t events_=0; //Channel里fd所监听的事件。
    uint32_t revents_=0;    //Channel里fd发生的事件。
private:
    //回调函数。
    std::function<void()> read_cb_; //fd读事件的回调函数。
    std::function<void()> close_cb_;    //关闭fd的回调函数。
    std::function<void()> error_cb_;    //fd发生错误的回调函数。
    std::function<void()> write_cb_; //fd写事件的回调函数。

public:
    Channel(EventLoop* loop, int fd); //构造函数。传入ep和EventLoop。
    ~Channel(); //析构函数。不可对ep和fd进行操作。

    int fd();   //返回fd。
    
    void use_et();  //启用边缘触发。
    void enable_reading();  //epoll_wait()监听读事件。
    void disable_reading(); //epoll_wait()取消监听读事件。
    void enable_writing();  //epoll_wait()监听写事件。
    void disable_writing(); //epoll_wait()取消监听写事件。
    void set_inepoll(); //设置inepoll为true。
    void set_revents(uint32_t ev); //设置revents的值为ev。

    bool is_inepoll();  //返回inepoll。
    uint32_t events();  //返回events。
    uint32_t revents(); //返回revents。

    void handle_events();   //处理channel的事件。

    void set_readcb(std::function<void()> func);    //设置fd读事件的回调函数。 
    void set_closecb(std::function<void()> func);   //设置关闭fd的回调函数。
    void set_errorcb(std::function<void()> func);   //设置fd()发生错误的回调函数。
    void set_writecb(std::function<void()> func);    //设置fd写事件的回调函数。
};