#pragma once

// #include "Channel.h"
#include "EventLoop.h"
#include <functional>

class Acceptor {
private:
    Socket *servsock_;
    Channel *acceptchannel_;
    EventLoop *loop_;
    std::function<void(Socket*)> newconncallback_;
public:
    Acceptor(EventLoop *loop, const std::string &ip, uint16_t port);
    ~Acceptor();
    void newconnection();
    void setnewconncallback(std::function<void(Socket*)>);
};