#include"EventLoop.h"


EventLoop::EventLoop():ep_(new Epoll)
{}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::run()
{
    //事件循环。
    while(true)
    {
        //channels保存返回的发生事件的通道。
        //通道channel保存着epoll_wait()返回的事件信息。
        std::vector<Channel*> channels=ep_->loop();

        for(auto ch:channels)
        {
            ch->handle_events();
        }
    }
}

void EventLoop::update_channel(Channel* ch)
{
    ep_->update_channel(ch);
}