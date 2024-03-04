#include <iostream>

#include "InetAddress.h"
#include "Socket.h"
#include "EventLoop.h"

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

    std::cout << "监听套接字文件描述符：" << listensock.fd() << std::endl;

    EventLoop loop;
    Channel * servchannel = (Channel*)new Channel(listensock.fd(), loop.getepoll());
    servchannel->enablereading();
    servchannel->setcallback(std::bind(&Channel::newconnection, servchannel, listensock));

    loop.run();
}