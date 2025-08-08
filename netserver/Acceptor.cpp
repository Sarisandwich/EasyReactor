#include"Acceptor.h"


Acceptor::Acceptor(EventLoop* loop, const std::string& ip, uint16_t port):loop_(loop)
{
    //创建服务端用于监听的listenfd。
    servsock_=new Socket(create_nonblocking_fd());

    //设置listenfd的属性。
    servsock_->set_reuseaddr();
    servsock_->set_nodelay();
    servsock_->set_reuseport();
    servsock_->set_keepalive();

    InetAddress servaddr(ip, port);

    //IP和端口绑定到socket。
    servsock_->bind(servaddr);

    //开启监听。缺省值为128。
    servsock_->listen();

    //创建epoll句柄（红黑树）。epoll句柄在EventLoop的构造函数中创建。private成员中已构造。
    //EventLoop loop;

    //创建服务端的channel，让channel里的listenfd监听读事件，将信息加入红黑树。
    acceptchannel_=new Channel(loop_, servsock_->fd());
    acceptchannel_->set_readcb(std::bind(&Acceptor::newConnection, this));
    acceptchannel_->enable_reading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}

void Acceptor::newConnection()
{
    InetAddress clientaddr;
    Socket* clientsock=new Socket(servsock_->accept(clientaddr));
    clientsock->set_ipport(clientaddr.ip(), clientaddr.port());

    newConnection_cb_(clientsock);
}

void Acceptor::set_newConnection_cb(std::function<void(Socket*)> func)
{
    newConnection_cb_=func;
}