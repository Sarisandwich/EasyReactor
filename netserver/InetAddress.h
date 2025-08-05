#pragma once

#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>

//socket的地址协议类。
class InetAddress
{
private:
    sockaddr_in addr_;
public:
    InetAddress();  //默认构造函数。
    InetAddress(const std::string& ip, uint16_t port);  //listenfd的构造函数。
    InetAddress(const sockaddr_in addr);    //clientfd的构造函数。
    ~InetAddress();

    const char* ip() const; //返回字符串表示的ip。形式为xxx.xxx.xxx.xxx。
    uint16_t port() const;  //返回整数表示的端口。
    const sockaddr* addr() const;   //返回addr_的地址。被转换为sockaddr。
    void set_addr(const sockaddr_in& clientaddr); //外部传入addr。
};