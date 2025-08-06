#include"TcpServer.h"


TcpServer::TcpServer(const std::string& ip, uint16_t port)
{
    //创建服务端用于监听的listenfd。
    Socket* servsock=new Socket(create_nonblocking_fd());

    //设置listenfd的属性。
    servsock->set_reuseaddr();
    servsock->set_nodelay();
    servsock->set_reuseport();
    servsock->set_keepalive();

    InetAddress servaddr(ip, port);

    //IP和端口绑定到socket。
    servsock->bind(servaddr);

    //开启监听。缺省值为128。
    servsock->listen();

    //创建epoll句柄（红黑树）。epoll句柄在EventLoop的构造函数中创建。private成员中已构造。
    //EventLoop loop;

    //创建服务端的channel，让channel里的listenfd监听读事件，将信息加入红黑树。
    Channel* servchannel=new Channel(loop_.ep(), servsock->fd());
    servchannel->set_readcb(std::bind(&Channel::new_connection, servchannel, servsock));
    servchannel->enable_reading();
}

TcpServer::~TcpServer()
{}

void TcpServer::start()
{
    loop_.run();
}