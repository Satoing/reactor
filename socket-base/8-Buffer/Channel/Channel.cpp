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

void Channel::enablewriting() {
    events_ |= EPOLLOUT;
    loop_->getepoll()->updateChannel(this);
}

void Channel::disablewriting() {
    events_ &= ~EPOLLOUT;
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

void Channel::handleevent() {
    if(revents_ & EPOLLHUP) {
        closecallback_();
    }
    else if(revents_ & EPOLLIN) {
        readcallback_();
    }
    else if(revents_ & EPOLLOUT) {
        writecallback_();
    }
    else {
        errorcallback_();
    }
}

void Channel::setreadcallback(std::function<void()> fn) {
    readcallback_ = fn;
}

void Channel::setwritecallback(std::function<void()> fn) {
    writecallback_ = fn;
}

void Channel::seterrorcallback(std::function<void()> fn) {
    errorcallback_ = fn;
}

void Channel::setclosecallback(std::function<void()> fn) {
    closecallback_ = fn;
}