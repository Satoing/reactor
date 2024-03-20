#pragma once

#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include <functional>

class EventLoop;

class Channel {
private:
    int fd_ = -1;
    EventLoop *loop_ = nullptr;
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    std::function<void()> readcallback_;
    std::function<void()> writecallback_;
    std::function<void()> errorcallback_;
    std::function<void()> closecallback_;
public:
    Channel(int fd, EventLoop *loop);
    ~Channel();

    void useet();
    void enablereading();
    void enablewriting();
    void disablewriting();
    void setinepoll();
    void setrevents(uint32_t events);

    int fd();
    int inepoll();
    uint32_t events();
    uint32_t revents();
    void handleevent();

    void setreadcallback(std::function<void()> fn);
    void setwritecallback(std::function<void()> fn);
    void seterrorcallback(std::function<void()> fn);
    void setclosecallback(std::function<void()> fn);
};