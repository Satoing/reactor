#pragma once

// #include "Channel.h"
#include "EventLoop.h"

class Acceptor {
private:
    Socket *servsock_;
    Channel *acceptchannel_;
    EventLoop *loop_;
public:
    Acceptor(EventLoop *loop, const std::string &ip, uint16_t port);
    ~Acceptor();
};