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

    // sockaddr_in servaddr;    //服务器网址结构体。
    // servaddr.sin_family=AF_INET;    //IPv4网络协议套接字类型。
    // servaddr.sin_addr.s_addr=inet_addr(argv[1]);    //服务端用于监听的IP地址。
    // servaddr.sin_port=htons(atoi(argv[2])); //服务端用于监听的端口。
    InetAddress servaddr(argv[1], atoi(argv[2]));

    //IP和端口绑定到socket。
    servsock.bind(servaddr);

    //开启监听。缺省值为128。
    servsock.listen();

    //创建epoll句柄（红黑树）。
    int epollfd=epoll_create(1);

    //为listenfd准备读事件。
    epoll_event ev;
    ev.data.fd=servsock.fd();
    ev.events=EPOLLIN;

    //需要监视的listenfd加入红黑树。
    epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock.fd(), &ev);

    //存放epoll_wait()返回事件的数组。
    epoll_event evs[10];

    //事件循环。
    while(true)
    {
        //等待监听的fd有事件发生。
        int infds=epoll_wait(epollfd, evs, 10, -1);

        //返回失败。
        if(infds<0) {perror("epoll_wait() failed."); break;}
        //超时。
        if(infds==0) {perror("epoll_wait() timeout."); break;}

        //infds>0说明有事件发生的fd的数量。
        for(int i=0;i<infds;++i)
        {
            if(evs[i].data.fd==servsock.fd()) //如果是listenfd有事件，说明有新的客户端连接。
            {
                InetAddress clientaddr;
                Socket* clientsock=new Socket(servsock.accept(clientaddr));

                printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                //为新客户端连接准备读事件，添加到红黑树。
                ev.data.fd=clientsock->fd();
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
            }
            else    //如果是客户端连接的fd有事件。
            {
                if(evs[i].events & EPOLLRDHUP)  //对方关闭连接。
                {
                    printf("client(fd=%d) closed connection.\n", evs[i].data.fd);
                    close(evs[i].data.fd);
                }
                else if(evs[i].events & (EPOLLIN|EPOLLPRI))    //读事件。缓冲区有数据可读。
                {
                    char buffer[1024];
                    while(true)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t nread=recv(evs[i].data.fd, buffer, sizeof(buffer)-1, 0);
                        if(nread>0)
                        {
                            printf("recv(clientfd=%d) message: %s\n", evs[i].data.fd, buffer);
                            send(evs[i].data.fd, buffer, sizeof(buffer), 0);
                        }
                        else if(nread==0)
                        {
                            printf("client(fd=%d) closed connection.\n", evs[i].data.fd);
                            close(evs[i].data.fd);
                            break;
                        }
                        else
                        {
                            if(errno==EAGAIN||errno==EWOULDBLOCK)
                            {
                                break;
                            }
                            else
                            {
                                perror("recv() failed.\n");
                                close(evs[i].data.fd);
                                break;
                            }
                        }
                    }
                }
                else if(evs[i].events & EPOLLOUT)   //写事件。缓冲区可写。
                {}
                else    //发生错误。
                {
                    printf("client(fd=%d) error.\n", evs[i].data.fd);
                    close(evs[i].data.fd);
                }
            }
        }
    }
}