#include"Epoll.h"

//Epoll类。
// class Epoll
// {
// private:
//     static const int MaxEvent=100;
//     int epollfd_=-1;    //epoll句柄。在构造函数初始化。
//     epoll_event evs_[MaxEvent]; //存放epoll_wait()返回的数组。

// public:
//     Epoll();    //构造函数。创建epollfd_。
//     ~Epoll();   //析构。关闭epollfd_。

//     void add_fd(int fd, uint16_t op);   //将fd与监听事件加入红黑树。
//     std::vector<epoll_event> loop(int timeout=-1);  //运行epollwait()。返回发生的事件。
// };

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

void Epoll::add_fd(int fd, uint32_t op)
{
    epoll_event ev;
    ev.data.fd=fd;
    ev.events=op;

    //加入红黑树。
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
}

std::vector<epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> ret_evs;

    bzero(evs_, sizeof(evs_));

    //等待监听的fd有事件发生。
    int infds=epoll_wait(epollfd_, evs_, MaxEvent, timeout);

    //返回失败。
    if(infds<0) {printf("%s:%s:%d epoll_wait() failed.error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno); exit(-1);}
    //超时。
    if(infds==0) {perror("epoll_wait() timeout."); return ret_evs;}

    //infds>0说明有事件发生的fd的数量。
    for(int i=0;i<infds;++i)
    {
        ret_evs.emplace_back(evs_[i]);
    }
    return ret_evs;
}