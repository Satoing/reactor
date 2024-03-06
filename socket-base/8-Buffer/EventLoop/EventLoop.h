#pragma once

#include "Epoll.h"

class Epoll;

class EventLoop {
private:
    Epoll *ep_;
public:
    EventLoop();
    ~EventLoop();
    Epoll* getepoll();
    void run();
};