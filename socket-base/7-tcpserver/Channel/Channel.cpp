#include "Channel.h"

Channel::Channel(int fd, EventLoop *loop): fd_(fd), loop_(loop) {}

Channel::~Channel() {}

void Channel::useet() {
    events_ |= EPOLLET;
}

void Channel::enablereading() {
    events_ |= EPOLLIN;
    loop_->getepoll()->updateChannel(this);
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
               closecallback_();
            }
            break;
        }
        ptr += readn;
    }
}

void Channel::handleevent() {
    if(revents_ & EPOLLHUP) {
        closecallback_();
    }
    else if(revents_ & EPOLLIN) {
        readcallback_();
    }
    else if(revents_ & EPOLLOUT) {}
    else {
        errorcallback_();
    }
}

void Channel::setreadcallback(std::function<void()> fn) {
    readcallback_ = fn;
}

void Channel::seterrorcallback(std::function<void()> fn) {
    errorcallback_ = fn;
}

void Channel::setclosecallback(std::function<void()> fn) {
    closecallback_ = fn;
}