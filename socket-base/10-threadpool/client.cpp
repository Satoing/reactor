#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    printf("connect ok\n");

    char buf[1024];
    for(int i = 0; i < 1; i++) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "这是第%d个消息", i+1);
        // 向char*中写入数字
        char tempbuf[1024] = {0};
        int len = strlen(buf);
        memcpy(tempbuf, &len, 4);
        memcpy(tempbuf+4, buf, len);

        send(sockfd, tempbuf, len+4, 0);
        // printf("发送数据: %s\n", buf);
    }
    for(int i = 0; i < 100; i++) {
        // 从char*中读取数字
        int len;
        recv(sockfd, &len, 4, 0);
        
        memset(buf, 0, sizeof(buf));
        recv(sockfd, buf, len, 0);
        printf("收到数据: %s\n", buf);
    }
    close(sockfd);
}