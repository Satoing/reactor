#pragma once

#include "Epoll.h"

class Epoll;
// Channel包含了所有需要的内容
class Channel {
private:
    int fd_ = -1;
    Epoll *ep_ = nullptr;
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;

public:
    Channel(int fd, Epoll *ep);
    ~Channel();

    void useet();
    void enablereading();
    void setinepoll();
    void setrevents(uint32_t events);

    int fd();
    int inepoll();
    uint32_t events();
    uint32_t revents();
};