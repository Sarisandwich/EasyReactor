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

    //创建epoll句柄（红黑树）。
    Epoll ep;

    //为listenfd准备读事件。需要监视的listenfd加入红黑树。
    ep.add_fd(servsock.fd(), EPOLLIN);

    //事件循环。
    while(true)
    {
        //evs保存返回的发生的事件。
        std::vector<epoll_event> evs;
        evs=ep.loop();

        for(auto& ev:evs)
        {
            if(ev.data.fd==servsock.fd()) //如果是listenfd有事件，说明有新的客户端连接。
            {
                InetAddress clientaddr;
                Socket* clientsock=new Socket(servsock.accept(clientaddr));

                printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                //为新客户端连接准备读事件，添加到红黑树。
                ep.add_fd(clientsock->fd(), EPOLLIN|EPOLLET);
            }
            else    //如果是客户端连接的fd有事件。
            {
                if(ev.events & EPOLLRDHUP)  //对方关闭连接。
                {
                    printf("client(fd=%d) closed connection.\n", ev.data.fd);
                    close(ev.data.fd);
                }
                else if(ev.events & (EPOLLIN|EPOLLPRI))    //读事件。缓冲区有数据可读。
                {
                    char buffer[1024];
                    while(true)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t nread=recv(ev.data.fd, buffer, sizeof(buffer)-1, 0);
                        if(nread>0)
                        {
                            printf("recv(clientfd=%d) message: %s\n", ev.data.fd, buffer);
                            send(ev.data.fd, buffer, sizeof(buffer), 0);
                        }
                        else if(nread==0)
                        {
                            printf("client(fd=%d) closed connection.\n", ev.data.fd);
                            close(ev.data.fd);
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
                                close(ev.data.fd);
                                break;
                            }
                        }
                    }
                }
                else if(ev.events & EPOLLOUT)   //写事件。缓冲区可写。
                {}
                else    //发生错误。
                {
                    printf("client(fd=%d) error.\n", ev.data.fd);
                    close(ev.data.fd);
                }
            }
        }
    }
}