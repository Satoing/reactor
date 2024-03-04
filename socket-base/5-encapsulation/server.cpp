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
#include <netinet/tcp.h>

#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"

int createnonblocking() {
    int listenfd;
    if( (listenfd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
        return -1;
    }
    return listenfd;
}

int main() {
    InetAddress servaddr("127.0.0.1", 5005);
    Socket listensock(createnonblocking());
    listensock.setkeepalive(true);
    listensock.setreuseaddr(true);
    listensock.setreuseport(true);
    listensock.settcpnodelay(true);
    listensock.bind(servaddr);
    listensock.listen(5);

    cout << "监听套接字文件描述符：" << listensock.fd() << endl;

    Epoll ep;
    Channel * servchannel = (Channel*)new Channel(listensock.fd(), &ep);
    servchannel->enablereading();

    while(true) {
        vector<Channel*> chs = ep.loop(); 

        for(auto ch : chs) {
            if(ch->revents() & EPOLLHUP) {
                cout << "1. 客户端断开连接" << endl;
                close(ch->fd());
            }
            else if(ch->revents() & EPOLLIN) {
                cout << "收到客户端的连接" << endl;
                if(ch->fd() == listensock.fd()) {
                    while(true) {
                        InetAddress clientaddr;
                        Socket *connsock = new Socket(listensock.accept(clientaddr));
                        if(connsock->fd() < 0 && errno == EAGAIN) break;
                        
                        cout << "connect client:" << clientaddr.ip() << ":" << clientaddr.port() 
                             << ", fd:" << connsock->fd() << endl;

                        Channel *clientchannel = (Channel*)new Channel(connsock->fd(), &ep);
                        clientchannel->enablereading();
                    }
                    continue;
                }
                // connfd上有读事件
                char buf[1024] = {0};
                int readn = 0;
                char *ptr = buf;

                while(true) {
                    if( (readn = recv(ch->fd(), ptr, 5, 0)) <= 0) {
                        // 读取完毕
                        if((readn < 0) && (errno == EAGAIN)) {
                            send(ch->fd(), buf, strlen(buf), 0);
                            cout << "发送数据：" << buf << endl;
                        } else {
                            cout << "2. 客户端断开连接" << endl;
                            close(ch->fd());
                        }
                        break;
                    }
                    ptr += readn;
                }
            }
            else if(ch->revents() & EPOLLOUT) {}
            else {
                cout << "3. 出现错误" << endl;
                close(ch->fd());
            }
        }
    }
}