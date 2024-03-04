#include "Channel.h"

Channel::Channel(int fd, Epoll *ep): fd_(fd), ep_(ep) {}

Channel::~Channel() {}

void Channel::useet() {
    events_ |= EPOLLET;
}

void Channel::enablereading() {
    events_ |= EPOLLIN;
    ep_->updateChannel(this);
}

void Channel::setinepoll() {
    inepoll_ = true;
}

void Channel::setrevents(uint32_t events) {
    revents_ = events;
}

int Channel::fd() {
    return fd_;
}

int Channel::inepoll() {
    return inepoll_;
}

uint32_t Channel::events() {
    return events_;
}

uint32_t Channel::revents() {
    return revents_;
}

void Channel::newconnection(Socket &sock) {
    while(true) {
        InetAddress clientaddr;
        Socket *connsock = new Socket(sock.accept(clientaddr));
        if(connsock->fd() < 0 && errno == EAGAIN) break;
        
        std::cout << "connect client:" << clientaddr.ip() << ":" << clientaddr.port() 
                << ", fd:" << connsock->fd() << std::endl;

        Channel *clientchannel = (Channel*)new Channel(connsock->fd(), ep_);
        clientchannel->setcallback(std::bind(&Channel::onmessage, clientchannel));
        clientchannel->useet();
        clientchannel->enablereading();
    }
    return;
}

void Channel::onmessage() {
    char buf[1024] = {0};
    int readn = 0;
    char *ptr = buf;

    while(true) {
        if( (readn = recv(fd_, ptr, 5, 0)) <= 0) {
            // 读取完毕
            if((readn < 0) && (errno == EAGAIN)) {
                send(fd_, buf, strlen(buf), 0);
                std::cout << "发送数据：" << buf << std::endl;
            } else {
                std::cout << "2. 客户端断开连接" << std::endl;
                close(fd_);
            }
            break;
        }
        ptr += readn;
    }
}

void Channel::handleevent() {
    if(revents_ & EPOLLHUP) {
        std::cout << "1. 客户端断开连接" << std::endl;
        close(fd_);
    }
    else if(revents_ & EPOLLIN) {
        callback_();
    }
    else if(revents_ & EPOLLOUT) {}
    else {
        std::cout << "3. 出现错误" << std::endl;
        close(fd_);
    }
}

void Channel::setcallback(std::function<void()> fn) {
    callback_ = fn;
}