#include"Channel.h"


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

void Channel::handle_events(Socket* servsock)
{
    if(fd_==servsock->fd()) //如果是listenfd有事件，说明有新的客户端连接。
    {
        InetAddress clientaddr;
        Socket* clientsock=new Socket(servsock->accept(clientaddr));

        printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

        //为新客户端连接准备读事件，添加到红黑树。
        Channel* clientchannel=new Channel(ep_, clientsock->fd());
        clientchannel->use_et();
        clientchannel->enable_reading();
    }
    else    //如果是客户端连接的fd有事件。
    {
        if(revents_ & EPOLLRDHUP)  //对方关闭连接。
        {
            printf("client(fd=%d) closed connection.\n", fd_);
            close(fd_);
        }
        else if(revents_ & (EPOLLIN|EPOLLPRI))    //读事件。缓冲区有数据可读。
        {
            char buffer[1024];
            while(true)
            {
                memset(buffer, 0, sizeof(buffer));
                ssize_t nread=recv(fd_, buffer, sizeof(buffer)-1, 0);
                if(nread>0)
                {
                    printf("recv(clientfd=%d) message: %s\n", fd_, buffer);
                    send(fd_, buffer, sizeof(buffer), 0);
                }
                else if(nread==0)
                {
                    printf("client(fd=%d) closed connection.\n", fd_);
                    close(fd_);
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
                        close(fd_);
                        break;
                    }
                }
            }
        }
        else if(revents_ & EPOLLOUT)   //写事件。缓冲区可写。
        {}
        else    //发生错误。
        {
            printf("client(fd=%d) error.\n", fd_);
            close(fd_);
        }
    }
}