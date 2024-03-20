#pragma once

// #include "Channel.h"
#include "EventLoop.h"

class Connection {
private:
    Socket *connsock_;
    Channel *connchannel_;
    EventLoop *loop_;
    std::function<void(Connection*)> errorcallback_;
    std::function<void(Connection*)> closecallback_;

public:
    Connection(EventLoop *loop, Socket *clientsock);
    ~Connection();
    std::string ip() const;
    uint16_t port() const;
    int fd() const;

    void errorcallback();
    void closecallback();

    void seterrorcallback(std::function<void(Connection*)> fn);
    void setclosecallback(std::function<void(Connection*)> fn);
};