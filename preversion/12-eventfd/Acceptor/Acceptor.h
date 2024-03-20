#pragma once

// #include "Channel.h"
#include "EventLoop.h"
#include <functional>
#include "Connection.h"

class Acceptor {
private:
    EventLoop *loop_;
    Socket servsock_;
    Channel acceptchannel_;
    std::function<void(std::unique_ptr<Socket>)> newconncallback_;
    
public:
    Acceptor(EventLoop *loop, const std::string &ip, uint16_t port);
    ~Acceptor();
    void newconnection();
    void setnewconncallback(std::function<void(std::unique_ptr<Socket>)> fn);
};