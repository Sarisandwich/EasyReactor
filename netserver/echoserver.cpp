//echoserver.cpp
//回显服务器。

#include<signal.h>
#include"EchoServer.h"

EchoServer* echoserver;

void Stop(int sig)  //信号2和信号15的处理函数，功能是停止服务程序。
{
    printf("sig=%d\n", sig);
    //调用EchoServer::Stop()停止服务。
    echoserver->Stop();
    printf("echoserver已停止。\n");
    delete echoserver;

    exit(0);
}

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./echoserver ip port.\n");
        printf("example: ./echoserver 172.20.15.230 5005.\n\n");
        return -1;
    }
    
    signal(SIGTERM, Stop);  //kill或killall命令默认发送的信号。
    signal(SIGINT, Stop);   //CTRL+C发出的信号。

    // TcpServer tcpserver(argv[1], atoi(argv[2]));
    echoserver=new EchoServer(argv[1], atoi(argv[2]), 3, 3);

    //运行事件循环。
    // tcpserver.start();
    echoserver->Start();

    return 0;
}