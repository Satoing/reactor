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
    acceptchannel_ = (Channel*)new Channel(servsock_->fd(), loop_);
    acceptchannel_->enablereading();
    acceptchannel_->setreadcallback(std::bind(&Acceptor::newconnection, this));
}

Acceptor::~Acceptor() {
    delete servsock_;
    delete acceptchannel_;
}

#include "Connection.h"

void Acceptor::newconnection() {
    while(true) {
        InetAddress clientaddr;
        Socket *connsock = new Socket(servsock_->accept(clientaddr));
        connsock->setipport(clientaddr.ip(), clientaddr.port());
        if(connsock->fd() < 0 && errno == EAGAIN) break;

        // 回调函数
        newconncallback_(connsock);
    }
}

void Acceptor::setnewconncallback(std::function<void(Socket*)> fn) {
    newconncallback_ = fn;
}