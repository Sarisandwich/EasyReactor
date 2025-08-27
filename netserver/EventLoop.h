#pragma once

#include"Epoll.h"
#include<memory>
#include<syscall.h>
#include<unistd.h>
#include<queue>
#include<mutex>
#include<sys/eventfd.h>
#include<sys/timerfd.h>

class Channel;
class Epoll;

//事件循环类。
class EventLoop
{
private:
    std::unique_ptr<Epoll> ep_; //每个事件循环只对应一个ep。
    std::function<void(EventLoop*)> epollTimeout_cb_;   //回调函数。epoll_wait()超时的处理。

    pid_t threadid_;    //事件循环所在线程的id。

    int wakeup_fd_; //用于唤醒事件循环线程的fd。
    std::queue<std::function<void()>> taskqueue_;   //事件循环线程被eventfd唤醒后，执行的任务队列。
    std::mutex mtx_;    //任务队列的互斥锁。
    std::unique_ptr<Channel> wakeChannel_;  //eventfd的channel。

    int timerfd_;   //定时器的fd。
    std::unique_ptr<Channel> timerChannel_; //定时器的Channel。
public:
    EventLoop();    //构造函数。创建ep。
    ~EventLoop();    //析构函数。销毁ep。

    void run(); //运行事件循环。

    void update_channel(Channel* ch);   //加入或修改红黑树内容。
    void removeChannel(Channel* ch);    //从红黑树删除channel。

    void set_epollTimeoutcb(std::function<void(EventLoop*)>);   //设置epollTimeout_cb_。

    bool isinLoopthread();    //判断当前线程是否为事件循环线程。

    void enqueueLoop(std::function<void()> func); //把任务添加到队列中。
    void wakeup();  //用eventfd唤醒事件循环线程。
    void handleWakeup();    //事件循环线程被唤醒后执行的操作。
    void handleTimer();     //定时器时间到了之后执行的操作。
};