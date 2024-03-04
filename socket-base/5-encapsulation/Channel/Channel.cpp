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