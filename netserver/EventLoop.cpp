#include"EventLoop.h"


EventLoop::EventLoop()
            :ep_(std::make_unique<Epoll>()), wakeup_fd_(eventfd(0, EFD_NONBLOCK)),
            wakeChannel_(std::make_unique<Channel>(this, wakeup_fd_))
{
    wakeChannel_->set_readcb(std::bind(&EventLoop::handleWakeup, this));
    wakeChannel_->enable_reading();
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