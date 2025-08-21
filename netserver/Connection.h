#pragma once

#include<functional>

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"InetAddress.h"
#include"Buffer.h"

#include<memory>
#include<atomic>

class Connection;
using spConnection=std::shared_ptr<Connection>;


//封装客户端连接的Connection。
class Connection:public std::enable_shared_from_this<Connection>
{
private:
    const std::unique_ptr<EventLoop>& loop_;   //Connection对应的事件循环。外部传入。
    std::unique_ptr<Socket> clientsock_;  //与客户端通讯的socket。外部传入。虽然是外部传入，但逻辑上仍属于类内成员，需要由Connection析构释放。
    std::unique_ptr<Channel> clientchannel_;    //Connection对应的channel。构造函数创建。类内成员。

    Buffer inputbuffer_;    //接收缓冲区。
    Buffer outputbuffer_;   //发送缓冲区。

    std::atomic<bool> disconnected_{false}; //是否已断开连接。由IO线程创建或修改，由工作线程判断。
private:
    //回调函数。
    std::function<void(spConnection)> close_cb_; //关闭fd的回调函数。
    std::function<void(spConnection)> error_cb_; //fd发生错误的回调函数。
    std::function<void(spConnection, std::string&)> onmessage_cb_;    //回调函数。在Connection::onmessage中对inputbuffer_里的数据进行处理。
    std::function<void(spConnection)> sendComplete_cb_; //回调函数。发送数据完成之后提醒TcpServer。
public:
    Connection(const std::unique_ptr<EventLoop>& loop, std::unique_ptr<Socket> clientsock);  //构造函数。
    ~Connection();    //析构函数。

    int fd() const; //返回fd。
    std::string ip() const; //返回ip。
    uint16_t port() const;  //返回端口。

    void onmessage();   //处理对端发送过来的报文。
    void send(const char* data, size_t size);    //发送数据。

    void set_closecb(std::function<void(spConnection)> func);    //设置关闭fd的回调函数。
    void set_errorcb(std::function<void(spConnection)> func);    //设置fd发生错误的回调函数。
    void set_onmessagecb(std::function<void(spConnection, std::string&)> func);   //设置onmessage_cb_。
    void set_sendCompletecb(std::function<void(spConnection)> func);    //设置sendComplete_cb_。

    void closeConnection(); //关闭客户端连接。供Channel回调。
    void errorConnection(); //客户端连接发生错误。供Channel回调。
    void writeCallback();   //处理写事件。供Channel回调。
};