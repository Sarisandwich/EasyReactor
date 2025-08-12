#include"Connection.h"


Connection::Connection(EventLoop* loop, Socket* clientsock):loop_(loop), clientsock_(clientsock)
{
    //为新客户端连接准备读事件，添加到红黑树。
    clientchannel_=new Channel(loop_, clientsock_->fd());
    clientchannel_->set_readcb(std::bind(&Connection::onmessage, this));
    clientchannel_->set_closecb(std::bind(&Connection::closeConnection, this));
    clientchannel_->set_errorcb(std::bind(&Connection::errorConnection, this));
    clientchannel_->use_et();
    clientchannel_->enable_reading();
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
    printf("Connection 被析构。\n");
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

void Connection::onmessage()
{
    char buffer[1024];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t nread=recv(fd(), buffer, sizeof(buffer)-1, 0);
        if(nread>0)    //成功读取数据。
        {
            // printf("recv(clientfd=%d) message: %s\n", fd(), buffer);
            // send(fd(), buffer, sizeof(buffer), 0);
            inputbuffer_.append(buffer, nread);
        }
        else if(nread==0)   //客户端连接已断开。
        {
            closeConnection();
            break;
        }
        else
        {
            if(errno==EAGAIN||errno==EWOULDBLOCK)   //数据读取完毕。
            {
                printf("recv(clientfd=%d) message: %s\n", fd(), inputbuffer_.data());
                ///////////////////////////////////////////
                //这个位置，对inputbuffer_里的数据经过某些处理。
                ///////////////////////////////////////////

                outputbuffer_=inputbuffer_;
                inputbuffer_.clear();
                send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);

                break;
            }
            else if(errno==EINTR)   //读取数据的时候被信号中断，继续读取。
            {
                continue;
            }
            else
            {
                perror("recv() failed.\n");
                close(fd());
                break;
            }
        }
    }
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