#pragma once

#include<functional>

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"InetAddress.h"
#include"Buffer.h"

//封装客户端通讯的Channel。
class Connection
{
private:
    EventLoop* loop_;   //Connection对应的事件循环。外部传入。
    Socket* clientsock_;  //与客户端通讯的socket。外部传入。虽然是外部传入，但逻辑上仍属于类内成员，需要由Connection析构释放。
    Channel* clientchannel_;    //Connection对应的channel。构造函数创建。类内成员。

    Buffer inputbuffer_;    //接收缓冲区。
    Buffer outputbuffer_;   //发送缓冲区。
private:
    //回调函数。
    std::function<void(Connection*)> close_cb_; //关闭fd的回调函数。
    std::function<void(Connection*)> error_cb_; //fd发生错误的回调函数。
public:
    Connection(EventLoop* loop, Socket* clientsock);  //构造函数。
    ~Connection();    //析构函数。

    int fd() const; //返回fd。
    std::string ip() const; //返回ip。
    uint16_t port() const;  //返回端口。

    void onmessage();   //处理对端发送过来的报文。

    void set_closecb(std::function<void(Connection*)> func);    //设置关闭fd的回调函数。
    void set_errorcb(std::function<void(Connection*)> func);    //设置fd发生错误的回调函数。

    void closeConnection(); //关闭客户端连接。供Channel回调。
    void errorConnection(); //客户端连接发生错误。供Channel回调。
};