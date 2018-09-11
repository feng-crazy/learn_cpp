#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define MYPORT  6102
#define BUFFER_SIZE 1024
int main()
{
    int sock_cli;
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd;

    ///定义sockfd
    sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip


    //连接服务器，成功返回0，错误返回-1
    while (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }



    while(1)
    {
    	int len;
		char sendbuf[BUFFER_SIZE] = "hello I am client\n";
		len = send(sock_cli, sendbuf, strlen(sendbuf),0); //发送
		printf("send len = %d\n",len);
		memset(sendbuf, 0, sizeof(sendbuf));


        char recvbuf[BUFFER_SIZE] = {0x00};

        len = recv(sock_cli, recvbuf, sizeof(recvbuf),0);
        printf("recv len = %d\n",len);

        if(len>0)
        	printf("%s", recvbuf);

        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock_cli);
    return 0;
}
