#include"EventLoop.h"


EventLoop::EventLoop():ep_(std::make_unique<Epoll>())
{}

EventLoop::~EventLoop()
{

}


#include<syscall.h>
#include<unistd.h>
void EventLoop::run()
{
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
            for(auto ch:channels)
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