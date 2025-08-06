#include"Channel.h"

// class Channel
// {
// private:
//     int fd_=-1;
//     Epoll* ep_=nullptr; //Channel类需要外部传入fd和ep，Channel类对它们没有修改权限，只是需要它们的信息。
//     //Channel与fd是一对一，Channel与ep是多对一。
//     //使用Channel类，ev.data.ptr指向Channel，取代ev.data.fd，这样可以携带更多信息。

//     bool inepoll_=false;    //Channel是否已在红黑树上。
//     uint32_t events_=0; //Channel里fd所监听的事件。
//     uint32_t revents_=0;    //Channel里fd发生的事件。

// public:
//     Channel(Epoll* ep, int fd); //构造函数。传入ep和fd。
//     ~Channel(); //析构函数。不可对ep和fd进行操作。

//     int fd();   //返回fd。
    
//     void use_et();  //启用边缘触发。
//     void enable_reading();  //epoll_wait()监听读事件。
//     void set_inepoll(); //设置inepoll为true。
//     void set_revents(uint32_t ev); //设置revents的值为ev。

//     bool is_inepoll();  //返回inepoll。
//     uint32_t events();  //返回events。
//     uint32_t revents(); //返回revents。
// };

Channel::Channel(Epoll* ep, int fd):ep_(ep), fd_(fd)
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
    ep_->update_channel(this);
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