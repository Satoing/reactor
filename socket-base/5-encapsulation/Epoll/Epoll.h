#pragma once

#include <cstdio>          /* See NOTES */
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>
#include <vector>
#include <iostream>
#include "Channel.h"
using namespace std;


class Channel;

class Epoll {
private:
    static const int MaxEvents = 100;
    int epfd_;
    epoll_event events_[MaxEvents];
public:
    Epoll();
    ~Epoll();
    // void addfd(int fd, uint32_t op);
    void updateChannel(Channel *ch);  // 使用联合类型中的指针，将Channel类添加到红黑树上 
    // std::vector<epoll_event> loop(int timeout = -1);
    std::vector<Channel*> loop(int timeout = -1);
};