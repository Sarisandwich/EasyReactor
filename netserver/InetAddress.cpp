#include"InetAddress.h"

// class InetAddress
// {
// private:
//     sockaddr_in addr_;
// public:
//     InetAddress(const std::string& ip, uint16_t port);  //listenfd的构造函数。
//     InetAddress(const sockaddr_in addr);    //clientfd的构造函数。
//     ~InetAddress();

//     const char* ip() const; //返回字符串表示的ip。形式为xxx.xxx.xxx.xxx。
//     uint16_t port() const;  //返回整数表示的端口。
//     const sockaddr* addr() const;   //返回addr_的地址。被转换为sockaddr。
// };

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
    addr_.sin_family=AF_INET;
    addr_.sin_addr.s_addr=inet_addr(ip.c_str());
    addr_.sin_port=htons(port);
}

InetAddress::InetAddress(const sockaddr_in addr):addr_(addr)
{}

InetAddress::~InetAddress()
{}

const char* InetAddress::ip() const
{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress::port() const
{
    return ntohs(addr_.sin_port);
}

const sockaddr* InetAddress::addr() const
{
    return (sockaddr*)&addr_;
}