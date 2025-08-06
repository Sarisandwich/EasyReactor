#include"Epoll.h"


Epoll::Epoll()
{
    if((epollfd_=epoll_create(1))<0)
    {
        printf("%s:%s:%d epoll_create() failed.error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno); exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_);
} 

void Epoll::update_channel(Channel* ch)
{
    epoll_event ev;
    ev.data.ptr=ch;
    ev.events=ch->events();

    //加入红黑树。
    if(ch->is_inepoll()==true)
    {
        epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev);
    }
    else
    {
        epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev);
        ch->set_inepoll();
    }
}

std::vector<Channel*> Epoll::loop(int timeout)
{
    std::vector<Channel*> channels;

    bzero(evs_, sizeof(evs_));

    //等待监听的fd有事件发生。
    int infds=epoll_wait(epollfd_, evs_, MaxEvent, timeout);

    //返回失败。
    if(infds<0) {printf("%s:%s:%d epoll_wait() failed.error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno); exit(-1);}
    //超时。
    if(infds==0) {perror("epoll_wait() timeout."); return channels;}

    //infds>0说明有事件发生的fd的数量。
    for(int i=0;i<infds;++i)
    {
        Channel* ch=(Channel*)evs_[i].data.ptr;
        ch->set_revents(evs_[i].events);
        channels.emplace_back(ch);
    }
    return channels;
}