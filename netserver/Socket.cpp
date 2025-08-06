#include"Socket.h"


int create_nonblocking_fd()
{
    int listenfd=socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if(listenfd<0)
    {
        //perror("socket() failed.\n"); exit(-1);
        printf("%s:%s:%d Listenfd create error:%d.\n", __FILE__, __FUNCTION__, __LINE__, errno); exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd):fd_(fd)
{}

Socket::~Socket()
{
    ::close(fd_);
}

int Socket::fd() const
{
    return fd_;
}

void Socket::set_reuseaddr(bool on)
{
    int opt=on?1:0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::set_nodelay(bool on)
{
    int opt=on?1:0;
    setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::set_reuseport(bool on)
{
    int opt=on?1:0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::set_keepalive(bool on)
{
    int opt=on?1:0;
    setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void Socket::bind(const InetAddress& servaddr)
{
    if(::bind(fd_, servaddr.addr(), sizeof(sockaddr))<0)
    {
        perror("bind() failed."); close(fd_); exit(-1);
    }
}

void Socket::listen(int n)
{
    if(::listen(fd_, n)!=0)
    {
        printf("%s:%s:%d Socket::listen() error:%d.\n", __FILE__, __FUNCTION__, __LINE__, errno); exit(-1);
    }
}

int Socket::accept(InetAddress& clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len=sizeof(peeraddr);
    int clientfd=::accept4(fd_, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);

    clientaddr.set_addr(peeraddr);

    return clientfd;
}