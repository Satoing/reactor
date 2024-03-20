#include <iostream>
#include <string>
#include <cstring>
using namespace std;

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

int initserver(unsigned short port) {
    int listenfd;
    if( (listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        return -1;
    }
    if(listen(listenfd, 5) < 0) {
        return -1;
    }
    return listenfd;
}

int main() {
    int listenfd;
    if( (listenfd = initserver(5005)) < 0) {
        return -1;
    }
    cout << "监听套接字文件描述符：" << listenfd << endl;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(listenfd, &fds);

    int maxfd = listenfd;

    while(true) {
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        fd_set tempfds = fds;
        int nfds;
        if( (nfds = select(maxfd+1, &tempfds, NULL, NULL, &timeout)) < 0) {
            perror("select failed");
            return -1;
        }
        if(nfds == 0) {
            cout << "select timeout" << endl;
            continue;
        }
        int count = 0;
        for(int eventfd = 0; eventfd <= maxfd && count < nfds; eventfd++) {
            if(FD_ISSET(eventfd, &tempfds) == 0) continue;
            // 接受客户端连接
            if(eventfd == listenfd) {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
                if(connfd < 0) {
                    perror("accept failed");
                }
                cout << "connect client:" << inet_ntoa(clientaddr.sin_addr) << endl;
                FD_SET(connfd, &fds);
                if(connfd > maxfd) maxfd = connfd;
            }
            // 处理客户端发来的消息
            else {
                char buf[1024] = {0};
                if(recv(eventfd, buf, 1024, 0) <= 0) {
                    cout << "客户端的连接断开：" << eventfd << endl;
                    close(eventfd);
                    FD_CLR(eventfd, &fds);

                    if(maxfd == eventfd) {
                        for(int i = maxfd; i >= 0; i--) {
                            if(FD_ISSET(i, &fds) == 0) continue;
                            maxfd = i; break;
                        }
                    }
                } else {
                    cout << "收到消息：" << buf << endl;
                    send(eventfd, buf, strlen(buf), 0);
                }
            }
        }
    }
    return 0;
}