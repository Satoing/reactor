#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, uint16_t port): loop_(loop) {
    InetAddress servaddr(ip, port);
    servsock_ = new Socket(createnonblocking());
    servsock_->setkeepalive(true);
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
    servsock_->settcpnodelay(true);
    servsock_->bind(servaddr);
    servsock_->listen(5);

    std::cout << "监听套接字文件描述符：" << servsock_->fd() << std::endl;

    // EventLoop loop;
    Channel * servchannel = (Channel*)new Channel(servsock_->fd(), loop_);
    servchannel->enablereading();
    servchannel->setcallback(std::bind(&Channel::newconnection, servchannel, *servsock_));
}

Acceptor::~Acceptor() {
    delete servsock_;
    delete acceptchannel_;
}