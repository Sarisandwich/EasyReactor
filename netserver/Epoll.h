#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<strings.h>
#include<sys/epoll.h>
#include<vector>
#include<unistd.h>

//Epoll类。
class Epoll
{
private:
    static const int MaxEvent=100;
    int epollfd_=-1;    //epoll句柄。在构造函数初始化。
    epoll_event evs_[MaxEvent]; //存放epoll_wait()返回的数组。

public:
    Epoll();    //构造函数。创建epollfd_。
    ~Epoll();   //析构。关闭epollfd_。

    void add_fd(int fd, uint32_t op);   //将fd与监听事件加入红黑树。
    std::vector<epoll_event> loop(int timeout=-1);  //运行epollwait()。返回发生的事件。
};