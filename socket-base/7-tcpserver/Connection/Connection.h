#pragma once

// #include "Channel.h"
#include "EventLoop.h"

class Connection {
private:
    Socket *connsock_;
    Channel *connchannel_;
    EventLoop *loop_;
public:
    Connection(EventLoop *loop, Socket *clientsock);
    ~Connection();
};