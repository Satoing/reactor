#include "Epoll.h"

Epoll::Epoll() {
    epfd_ = epoll_create(1);
}

Epoll::~Epoll() {
    close(epfd_);
    epfd_ = -1;
}

int Epoll::epfd() {
    return epfd_;
}

// void Epoll::removechannel(Channel* ch) {
//     if(ch->inepoll()) {
//         if(epoll_ctl(ep_->epfd(), EPOLL_CTL_MOD, ch->fd(), &event) == -1) exit(-1);
//     }
// }

std::vector<Channel*> Epoll::loop(int timeout) {
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epfd_, events_, MaxEvents, timeout);
    std::vector<Channel*> chs;
    // 失败
    if(infds == -1) {
        perror("wait failed"); exit(-1);
    }
    // 超时，表示系统很空闲
    if(infds == 0) return chs;
    
    for(int i = 0; i < infds; i++) {
        Channel *ch = (Channel*)events_[i].data.ptr;
        ch->setrevents(events_[i].events);
        chs.emplace_back(ch);
    }
    return chs;
}