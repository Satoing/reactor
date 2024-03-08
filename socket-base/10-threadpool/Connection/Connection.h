#pragma once

// #include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class Connection {
private:
    Socket *connsock_;
    Channel *connchannel_;
    EventLoop *loop_;
    std::function<void(Connection*)> errorcallback_;
    std::function<void(Connection*)> closecallback_;
    std::function<void(Connection*, std::string&)> messagecallback_;
    std::function<void(Connection*)> sendcompletecallback_;
    Buffer inputbuffer_;
    Buffer outputbuffer_;

public:
    Connection(EventLoop *loop, Socket *clientsock);
    ~Connection();
    std::string ip() const;
    uint16_t port() const;
    int fd() const;

    void onmessage();
    void writecallback();
    void errorcallback();
    void closecallback();

    void seterrorcallback(std::function<void(Connection*)> fn);
    void setclosecallback(std::function<void(Connection*)> fn);
    void setmessagecallback(std::function<void(Connection*, std::string&)> fn);
    void setsendcompletecallback(std::function<void(Connection*)> fn);

    void send(const char *data, int size);
};