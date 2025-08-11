#include"Connection.h"


Connection::Connection(EventLoop* loop, Socket* clientsock):loop_(loop), clientsock_(clientsock)
{
    //为新客户端连接准备读事件，添加到红黑树。
    clientchannel_=new Channel(loop_, clientsock_->fd());
    clientchannel_->set_readcb(std::bind(&Channel::onmessage, clientchannel_));
    clientchannel_->set_closecb(std::bind(&Connection::closeConnection, this));
    clientchannel_->set_errorcb(std::bind(&Connection::errorConnection, this));
    clientchannel_->use_et();
    clientchannel_->enable_reading();
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}

int Connection::fd() const
{
    return clientsock_->fd();
}
std::string Connection::ip() const
{
    return clientsock_->ip();
}
uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::set_closecb(std::function<void(Connection*)> func)
{
    close_cb_=func;
}

void Connection::set_errorcb(std::function<void(Connection*)> func)
{
    error_cb_=func;
}

void Connection::closeConnection()
{
    close_cb_(this);
}

void Connection::errorConnection()
{
    error_cb_(this);
}