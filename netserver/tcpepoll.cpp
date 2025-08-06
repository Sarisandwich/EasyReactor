#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/fcntl.h>
#include<sys/epoll.h>
#include<netinet/tcp.h>

#include"InetAddress.h"
#include"Socket.h"
#include"Epoll.h"
#include"EventLoop.h"

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./tcpepoll ip port.\n");
        printf("example: ./tcpepoll 172.20.15.230 5005.\n\n");
        return -1;
    }
    
    //创建服务端用于监听的listenfd。
    Socket servsock(create_nonblocking_fd());

    //设置listenfd的属性。
    servsock.set_reuseaddr();
    servsock.set_nodelay();
    servsock.set_reuseport();
    servsock.set_keepalive();

    InetAddress servaddr(argv[1], atoi(argv[2]));

    //IP和端口绑定到socket。
    servsock.bind(servaddr);

    //开启监听。缺省值为128。
    servsock.listen();

    //创建epoll句柄（红黑树）。epoll句柄在EventLoop的构造函数中创建。
    EventLoop loop;

    //创建服务端的channel，让channel里的listenfd监听读事件，将信息加入红黑树。
    Channel* servchannel=new Channel(loop.ep(), servsock.fd());
    servchannel->set_readcb(std::bind(&Channel::new_connection, servchannel, &servsock));
    servchannel->enable_reading();

    //运行事件循环。
    loop.run();
}