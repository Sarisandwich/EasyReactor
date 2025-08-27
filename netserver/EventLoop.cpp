#include"EventLoop.h"

int createTimerfd(int sec=5)
{
    int tfd=timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
    struct itimerspec timeout;  //定时时间的数据。
    memset(&timeout, 0, sizeof(itimerspec));

    timeout.it_value.tv_sec=sec;
    timeout.it_value.tv_nsec=0;

    timerfd_settime(tfd, 0, &timeout, 0);
    return tfd;
}

EventLoop::EventLoop()
            :ep_(std::make_unique<Epoll>()), 
            wakeup_fd_(eventfd(0, EFD_NONBLOCK)), timerfd_(createTimerfd()),
            wakeChannel_(std::make_unique<Channel>(this, wakeup_fd_)),
            timerChannel_(std::make_unique<Channel>(this, timerfd_))
{
    wakeChannel_->set_readcb(std::bind(&EventLoop::handleWakeup, this));
    wakeChannel_->enable_reading();
    timerChannel_->set_readcb(std::bind(&EventLoop::handleTimer, this));
    timerChannel_->enable_reading();
}

EventLoop::~EventLoop()
{

}


void EventLoop::run()
{
    threadid_=syscall(SYS_gettid);  //获取事件循环所在的线程id。

    //事件循环。
    while(true)
    {
        //channels保存返回的发生事件的通道。
        //通道channel保存着epoll_wait()返回的事件信息。
        std::vector<Channel*> channels=ep_->loop(5*1000);
        if(channels.size()==0)
        {
            epollTimeout_cb_(this);
        }
        else
        {
            for(auto& ch:channels)
            {
                ch->handle_events();
            }
        }
    }
}

void EventLoop::update_channel(Channel* ch)
{
    ep_->update_channel(ch);
}

void EventLoop::removeChannel(Channel* ch)
{
    ep_->removeChannel(ch);
}

void EventLoop::set_epollTimeoutcb(std::function<void(EventLoop*)> func)
{
    epollTimeout_cb_=func;
}

bool EventLoop::isinLoopthread()
{
    return threadid_==syscall(SYS_gettid);
}

void EventLoop::enqueueLoop(std::function<void()> func)
{
    //任务入队。
    {
        std::lock_guard<std::mutex> lock(mtx_);
        taskqueue_.push(func);
    }
    //唤醒事件循环。
    wakeup();
}

void EventLoop::wakeup()
{
    uint64_t val=1;
    write(wakeup_fd_, &val, sizeof(val));
}

void EventLoop::handleWakeup()
{
    printf("handleWakeup() thread is %ld.\n", syscall(SYS_gettid));

    uint64_t val;
    read(wakeup_fd_, &val, sizeof(val));

    std::function<void()> func;

    
    std::lock_guard<std::mutex> lock(mtx_);

    while(!taskqueue_.empty())
    {
        func=std::move(taskqueue_.front());
        taskqueue_.pop();
        func();
    }
    
}

void EventLoop::handleTimer()
{
    //重新设定计时器。
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(itimerspec));
    timeout.it_value.tv_sec=5;
    timeout.it_value.tv_nsec=0;
    timerfd_settime(timerfd_, 0, &timeout, 0);

    printf("闹钟响了。\n");
}