#include "EventLoop.h"

EventLoop::EventLoop(): ep_(new Epoll) {}

EventLoop::~EventLoop() {}

void EventLoop::run() {
    // std::cout << "EventLoop run: " << syscall(SYS_gettid) << std::endl;
    while(true) {
        std::vector<Channel*> chs = ep_->loop(); 
        if(chs.size() == 0) epolltimeoutcallback_(this);
        for(auto ch : chs) ch->handleevent();
    }
}

void EventLoop::removechannel(Channel *ch) {
    if(ch->inepoll()) {
        if(epoll_ctl(ep_->epfd(), EPOLL_CTL_DEL, ch->fd(), 0) == -1) 
            exit(-1);
    }
}

void EventLoop::updateChannel(Channel *ch) {
    epoll_event event;
    event.data.ptr = ch;
    event.events = ch->events();
    
    if(ch->inepoll()) {
        if(epoll_ctl(ep_->epfd(), EPOLL_CTL_MOD, ch->fd(), &event) == -1) exit(-1);
    } else {
        if(epoll_ctl(ep_->epfd(), EPOLL_CTL_ADD, ch->fd(), &event) == -1) exit(-1);
    }
    ch->setinepoll();
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn) {
    epolltimeoutcallback_ = fn;
}