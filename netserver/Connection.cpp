#include"Connection.h"


Connection::Connection(EventLoop* loop, Socket* clientsock):loop_(loop), clientsock_(clientsock)
{
    //为新客户端连接准备读事件，添加到红黑树。
    clientchannel_=new Channel(loop_, clientsock_->fd());
    clientchannel_->set_readcb(std::bind(&Channel::onmessage, clientchannel_));
    clientchannel_->use_et();
    clientchannel_->enable_reading();
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}