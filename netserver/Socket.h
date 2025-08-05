#pragma once

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include"InetAddress.h"

//创建非阻塞的fd。
int create_nonblocking_fd();

//Socket类。
class Socket
{
private:
    const int fd_;
public:
    Socket(int fd); //构造函数，传入fd。
    ~Socket();

    int fd() const; //返回fd。
    void set_reuseaddr(bool on=true);    //设置REUSEADDR，默认为true。
    void set_nodelay(bool on=true);     //设置NODELAY。
    void set_reuseport(bool on=true);   //设置REUSEPORT。
    void set_keepalive(bool on=true);   //设置KEEPALIVE。

    void bind(const InetAddress& servaddr); //服务端的socket调用此函数。
    void listen(int n=128); //服务端的socket调用此函数。
    int accept(InetAddress& clientaddr);    //服务端的socket调用此函数。
};