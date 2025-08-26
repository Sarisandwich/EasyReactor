//echoserver.cpp
//回显服务器。

#include"EchoServer.h"

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./echoserver ip port.\n");
        printf("example: ./echoserver 172.20.15.230 5005.\n\n");
        return -1;
    }
    
    // TcpServer tcpserver(argv[1], atoi(argv[2]));
    EchoServer echoserver(argv[1], atoi(argv[2]), 3, 3);

    //运行事件循环。
    // tcpserver.start();
    echoserver.Start();

    return 0;
}