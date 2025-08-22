#include"Connection.h"


Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock):loop_(loop), clientsock_(std::move(clientsock))
{
    //为新客户端连接准备读事件，添加到红黑树。
    clientchannel_=std::make_unique<Channel>(loop_, clientsock_->fd());
    clientchannel_->set_readcb(std::bind(&Connection::onmessage, this));
    clientchannel_->set_closecb(std::bind(&Connection::closeConnection, this));
    clientchannel_->set_errorcb(std::bind(&Connection::errorConnection, this));
    clientchannel_->set_writecb(std::bind(&Connection::writeCallback, this));
    //clientchannel_->use_et();
    clientchannel_->enable_reading();
}

Connection::~Connection()
{

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
                while(true)
                {
                    //截取报文头部。若inputbuffer_的长度小于头部记录的报文长度，则说明报文不完整，先退出。
                    int len;
                    memcpy(&len, inputbuffer_.data(), 4);
                    if(inputbuffer_.size()<len+4) break;
                    std::string message(inputbuffer_.data()+4, len);   //读取报文内容。丢弃头部。
                    inputbuffer_.erase(0, len+4);   //清除inputbuffer_中已读取的内容。

                    ///////////////////////////////////////////
                    //这个位置，对inputbuffer_里的数据经过某些处理。
                    ///////////////////////////////////////////

                    printf("message (eventfd=%d): %s\n",fd(),message.c_str());

                    onmessage_cb_(shared_from_this(), message);
                }

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

void Connection::send(const char* data, size_t size)
{
    if(disconnected_.load()){return;}
    outputbuffer_.appendWithHead(data, size);
    clientchannel_->enable_writing();
}

void Connection::set_closecb(std::function<void(spConnection)> func)
{
    close_cb_=func;
}

void Connection::set_errorcb(std::function<void(spConnection)> func)
{
    error_cb_=func;
}

void Connection::set_onmessagecb(std::function<void(spConnection, std::string&)> func)
{
    onmessage_cb_=func;
}

void Connection::set_sendCompletecb(std::function<void(spConnection)> func)
{
    sendComplete_cb_=func;
}

void Connection::closeConnection()
{
    disconnected_.store(true);
    clientchannel_->remove();
    close_cb_(shared_from_this());
}

void Connection::errorConnection()
{
    disconnected_.store(true);
    clientchannel_->remove();
    error_cb_(shared_from_this());
}

void Connection::writeCallback()
{
    int writen=::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if(writen>0)
    {
        outputbuffer_.erase(0, writen);
    }

    if(outputbuffer_.size()==0)
    {
        clientchannel_->disable_writing();
        sendComplete_cb_(shared_from_this());
    }
}