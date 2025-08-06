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

    //创建服务端的channel，让channel里的listenfd监听读事件，将信息加入红黑树。
    Channel* servchannel=new Channel(&ep, servsock.fd());
    servchannel->enable_reading();

    //事件循环。
    while(true)
    {
        //channels保存返回的发生事件的通道。
        //通道channel保存着epoll_wait()返回的事件信息。
        std::vector<Channel*> channels=ep.loop();

        for(auto ch:channels)
        {
            if(ch->fd()==servsock.fd()) //如果是listenfd有事件，说明有新的客户端连接。
            {
                InetAddress clientaddr;
                Socket* clientsock=new Socket(servsock.accept(clientaddr));

                printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                //为新客户端连接准备读事件，添加到红黑树。
                Channel* clientchannel=new Channel(&ep, clientsock->fd());
                clientchannel->use_et();
                clientchannel->enable_reading();
            }
            else    //如果是客户端连接的fd有事件。
            {
                if(ch->revents() & EPOLLRDHUP)  //对方关闭连接。
                {
                    printf("client(fd=%d) closed connection.\n", ch->fd());
                    close(ch->fd());
                }
                else if(ch->revents() & (EPOLLIN|EPOLLPRI))    //读事件。缓冲区有数据可读。
                {
                    char buffer[1024];
                    while(true)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t nread=recv(ch->fd(), buffer, sizeof(buffer)-1, 0);
                        if(nread>0)
                        {
                            printf("recv(clientfd=%d) message: %s\n", ch->fd(), buffer);
                            send(ch->fd(), buffer, sizeof(buffer), 0);
                        }
                        else if(nread==0)
                        {
                            printf("client(fd=%d) closed connection.\n", ch->fd());
                            close(ch->fd());
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
                                close(ch->fd());
                                break;
                            }
                        }
                    }
                }
                else if(ch->revents() & EPOLLOUT)   //写事件。缓冲区可写。
                {}
                else    //发生错误。
                {
                    printf("client(fd=%d) error.\n", ch->fd());
                    close(ch->fd());
                }
            }
        }
    }
}