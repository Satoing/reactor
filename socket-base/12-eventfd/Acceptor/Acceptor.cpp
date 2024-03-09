#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, uint16_t port)
         : loop_(loop), servsock_(createnonblocking()), acceptchannel_(servsock_.fd(), loop_) {
    InetAddress servaddr(ip, port);
    servsock_.setkeepalive(true);
    servsock_.setreuseaddr(true);
    servsock_.setreuseport(true);
    servsock_.settcpnodelay(true);
    servsock_.bind(servaddr);
    servsock_.listen(5);

    std::cout << "监听套接字文件描述符：" << servsock_.fd() << std::endl;

    acceptchannel_.enablereading();
    acceptchannel_.setreadcallback(std::bind(&Acceptor::newconnection, this));
}

Acceptor::~Acceptor() {}

void Acceptor::newconnection() {
    while(true) {
        InetAddress clientaddr;
        // 使用临时对象给unique_ptr赋初值
        std::unique_ptr<Socket> connsock(new Socket(servsock_.accept(clientaddr)));
        connsock->setipport(clientaddr.ip(), clientaddr.port());
        if(connsock->fd() < 0 && errno == EAGAIN) break;

        // 回调函数
        newconncallback_(std::move(connsock));
    }
}

void Acceptor::setnewconncallback(std::function<void(std::unique_ptr<Socket>)> fn) {
    newconncallback_ = fn;
}