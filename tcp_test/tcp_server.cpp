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
#include <iostream>

#define PORT 6102
#define QUEUE 20

int main() {
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd;     //选择器

    /*创建socket*/
    int ss = socket(AF_INET, SOCK_STREAM, 0);   //AF_INET   IPV4   ;SOCK_STREAM   TCP
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(PORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*bind*/
    if(bind(ss, (struct sockaddr* ) &server_sockaddr, sizeof(server_sockaddr))==-1) {
        perror("bind");
        exit(1);
    }

    /*listen*/
    if(listen(ss, QUEUE) == -1) {
        perror("listen");
        exit(1);
    }
    /*connect*/
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    ///成功返回非负描述字，出错返回-1
    int conn = accept(ss, (struct sockaddr*)&client_addr, &length);   //目测需要客户端部分的addr
    if( conn < 0 ) {
        perror("connect");
        exit(1);
    }

    while(1)
    {
        char buffer[1024]={0x00};
        memset(buffer, 0 ,sizeof(buffer));
        int len = 0;
        len = recv(conn, buffer, sizeof(buffer),0);
		printf("recv len = %d\n",len);

		if(len>0)
			printf("%s", buffer);

        char buf[] = "hello i am server\n";
        len = send(conn, buf, sizeof(buf), 0);
        printf("send len = %d\n",len);

        usleep(500000);

    }
    close(conn);
    close(ss);
    return 0;
}
