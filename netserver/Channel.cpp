#include"Channel.h"


Channel::Channel(EventLoop* loop, int fd):loop_(loop), fd_(fd)
{}

Channel::~Channel()
{}

int Channel::fd()
{
    return fd_;
}

void Channel::use_et()
{
    events_|=EPOLLET;
}

void Channel::enable_reading()
{
    events_|=EPOLLIN;
    loop_->update_channel(this);
}

void Channel::disable_reading()
{
    events_&=~EPOLLIN;
    loop_->update_channel(this);
}

void Channel::enable_writing()
{
    events_|=EPOLLOUT;
    loop_->update_channel(this);
}

void Channel::disable_writing()
{
    events_&=~EPOLLOUT;
    loop_->update_channel(this);
}

void Channel::set_inepoll()
{
    inepoll_=true;
}

void Channel::set_revents(uint32_t ev)
{
    revents_=ev;
}

bool Channel::is_inepoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}

uint32_t Channel::revents()
{
    return revents_;
}

void Channel::handle_events()
{
    if(revents_ & EPOLLRDHUP)  //对方关闭连接。
    {
        close_cb_();
    }
    else if(revents_ & (EPOLLIN|EPOLLPRI))    //读事件。缓冲区有数据可读。
    {
        read_cb_();
    }
    else if(revents_ & EPOLLOUT)   //写事件。缓冲区可写。
    {
        write_cb_();
    }
    else    //发生错误。
    {
        error_cb_();
    }
}

void Channel::set_readcb(std::function<void()> func)
{
    read_cb_=func;
}

void Channel::set_closecb(std::function<void()> func)
{
    close_cb_=func;
}

void Channel::set_errorcb(std::function<void()> func)
{
    error_cb_=func;
}

void Channel::set_writecb(std::function<void()> func)
{
    write_cb_=func;
}