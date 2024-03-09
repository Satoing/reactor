#pragma once

#include "Epoll.h"
#include <functional>
#include <memory>

// #include <unistd.h>
// #include <sys/syscall.h>

class Epoll;
class Channel;

class EventLoop {
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;
public:
    EventLoop();
    ~EventLoop();
    
    void run();
    void removechannel(Channel *ch);
    void updateChannel(Channel *ch);  // 使用联合类型中的指针，将Channel类添加到红黑树上 
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);
};