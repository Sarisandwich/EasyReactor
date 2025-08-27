#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<time.h>

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./client ip port.\n");
        printf("example: ./client 172.20.15.230 5005.\n\n");
        return -1;
    }

    int sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("socket() failed.\n"); return -1;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);
    servaddr.sin_port=htons(atoi(argv[2]));

    if(connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))!=0)
    {
        printf("connect(%s:%s) failed.\n", argv[1], argv[2]); close(sockfd); return -1;
    }
    printf("connected.\n");

    char buffer[1024];
    for(int i=0;i<10;++i)
    {
        memset(buffer, 0, sizeof(buffer));
        // printf("input:"); scanf("%s", buffer);
        sprintf(buffer, "这是第%d个测试报文。", i);

        char tmpbuf[1024];
        memset(tmpbuf, 0, sizeof(tmpbuf));
        int len=strlen(buffer);
        memcpy(tmpbuf, &len, 4);
        memcpy(tmpbuf+4, buffer, len);


        if(send(sockfd, tmpbuf, len+4, 0)<=0)
        {
            printf("send() failed.\n");
            close(sockfd);
            return -1;
        }
    // }
    // for(int i=0;i<2;++i)
    // {
        // int len;
        recv(sockfd, &len, 4, 0);
        
        memset(buffer, 0, sizeof(buffer));
        if(recv(sockfd, buffer, len, 0)<=0)
        {
            printf("recv() failed.\n");
            close(sockfd);
            return -1;
        }
        printf("recv message:%s\n", buffer);
        sleep(1);
    }
    sleep(100);
    return 0;
}