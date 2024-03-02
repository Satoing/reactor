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
#include <sys/epoll.h>
#include <fcntl.h>

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

bool setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1) flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int listenfd;
    if( (listenfd = initserver(5005)) < 0) {
        return -1;
    }
    setnonblocking(listenfd);
    cout << "监听套接字文件描述符：" << listenfd << endl;

    int epfd = epoll_create(1);
    epoll_event event;
    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);

    epoll_event events[10];
    while(true) {
        int infds = epoll_wait(epfd, events, 10, -1);
        if(infds == -1) {
            perror("wait failed"); break;
        }
        if(infds == 0) {
            cout << "epoll timeout" << endl; continue;
        }
        for(int i = 0; i < infds; i++) {
            if(events[i].data.fd == listenfd) {
                while(true) {
                    struct sockaddr_in clientaddr;
                    socklen_t len = sizeof(clientaddr);
                    int connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
                    if(connfd < 0 && errno == EAGAIN) break;
                    setnonblocking(connfd);
                    cout << "connect client:" << inet_ntoa(clientaddr.sin_addr) << endl;

                    event.data.fd = connfd;
                    event.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
                }
            } else {
                char buf[1024] = {0};
                int readn = 0;
                char *ptr = buf;

                while(true) {
                    if( (readn = recv(events[i].data.fd, ptr, 5, 0)) <= 0) {
                        // 读取完毕
                        if((readn < 0) && (errno == EAGAIN)) {
                            send(events[i].data.fd, buf, strlen(buf), 0);
                            cout << "发送数据：" << buf << endl;
                        } else {
                            close(events[i].data.fd);
                        }
                        break;
                    }
                    ptr += readn;
                }
            }
        }
    }
}