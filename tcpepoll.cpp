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

//设置非阻塞IO。
void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);
}

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./tcpepoll ip port.\n");
        printf("example: ./tcpepoll 172.20.15.230 5005.\n\n");
        return -1;
    }
    
    //创建服务端用于监听的listenfd。
    int listenfd=socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd<0)
    {
        perror("socket() failed.\n"); return -1;
    }

    //设置listenfd的属性。
    int opt=1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof(opt)));

    //把服务器的listenfd设置为非阻塞。
    setnonblocking(listenfd);

    sockaddr_in servaddr;    //服务器网址结构体。
    servaddr.sin_family=AF_INET;    //IPv4网络协议套接字类型。
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);    //服务端用于监听的IP地址。
    servaddr.sin_port=htons(atoi(argv[2])); //服务端用于监听的端口。

    //IP和端口绑定到socket。
    if(bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))<0)
    {
        perror("bind() failed."); close(listenfd); return -1;
    }

    //开启监听。高并发网络服务器中第二个参数要调大。
    if(listen(listenfd, 128)!=0)
    {
        perror("listen() failed."); close(listenfd); return -1;
    }

    //创建epoll句柄（红黑树）。
    int epollfd=epoll_create(1);

    //为listenfd准备读事件。
    epoll_event ev;
    ev.data.fd=listenfd;
    ev.events=EPOLLIN;

    //需要监视的listenfd加入红黑树。
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

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
            if(evs[i].data.fd==listenfd) //如果是listenfd有事件，说明有新的客户端连接。
            {
                sockaddr_in clientaddr;
                socklen_t len=sizeof(clientaddr);
                int clientfd=accept(listenfd, (sockaddr*)&clientaddr, &len);
                setnonblocking(clientfd);

                printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

                //为新客户端连接准备读事件，添加到红黑树。
                ev.data.fd=clientfd;
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
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