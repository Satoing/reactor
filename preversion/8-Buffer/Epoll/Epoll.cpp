#include "Epoll.h"

Epoll::Epoll() {
    epfd_ = epoll_create(1);
}

Epoll::~Epoll() {
    close(epfd_);
    epfd_ = -1;
}

// void Epoll::addfd(int fd, uint32_t op) {
//     epoll_event event;
//     event.data.fd = fd;
//     event.events = EPOLLIN | EPOLLET; // 采用边缘触发模式
//     if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event) == -1) {
//         exit(-1);
//     }
// }

void Epoll::updateChannel(Channel *ch) {
    epoll_event event;
    event.data.ptr = ch;
    event.events = ch->events();
    
    if(ch->inepoll()) {
        if(epoll_ctl(epfd_, EPOLL_CTL_MOD, ch->fd(), &event) == -1) exit(-1);
    } else {
        if(epoll_ctl(epfd_, EPOLL_CTL_ADD, ch->fd(), &event) == -1) exit(-1);
    }
    ch->setinepoll();
}

std::vector<Channel*> Epoll::loop(int timeout) {
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epfd_, events_, MaxEvents, timeout);
    std::vector<Channel*> chs;
    if(infds == -1) {
        perror("wait failed"); exit(-1);
    }
    if(infds == 0) return chs;
    for(int i = 0; i < infds; i++) {
        Channel *ch = (Channel*)events_[i].data.ptr;
        ch->setrevents(events_[i].events);
        chs.emplace_back(ch);
    }
    return chs;
}

// std::vector<epoll_event> Epoll::loop(int timeout) {
//     bzero(events_, sizeof(events_));
//     int infds = epoll_wait(epfd_, events_, MaxEvents, timeout);
//     std::vector<epoll_event> events;
//     if(infds == -1) {
//         perror("wait failed"); exit(-1);
//     }
//     if(infds == 0) return events;
    
//     for(int i = 0; i < infds; i++) {
//         events.emplace_back(events_[i]);
//     }
//     return events;
// }