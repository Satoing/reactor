#include "EventLoop.h"

EventLoop::EventLoop(): ep_(new Epoll) {}

EventLoop::~EventLoop() {
    delete ep_;
}

Epoll* EventLoop::getepoll() {
    return ep_;
}

void EventLoop::run() {
    while(true) {
        std::vector<Channel*> chs = ep_->loop(); 
        for(auto ch : chs) {
            ch->handleevent();
        }
    }
}