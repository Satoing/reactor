#pragma once

#include "Epoll.h"
#include <functional>
#include <memory>
#include <queue>
#include <mutex>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/eventfd.h>

class Epoll;
class Channel;

class EventLoop {
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;
    pid_t tid_;                                 // 存放事件循环所在的线程id，在run函数中赋值
    std::queue<std::function<void()>> tasks_;   // 执行sendinloop的任务队列
    std::mutex mutex_;
    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_;      // 将wakeupfd加入事件循环中

public:
    EventLoop();
    ~EventLoop();
    
    void run();
    void removechannel(Channel *ch);
    void updateChannel(Channel *ch);  // 使用联合类型中的指针，将Channel类添加到红黑树上 
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);

    bool isinloopthread();            // 判断事件循环是否在IO线程中，否则在工作线程中
    void queueinloop(std::function<void()> fn);
    void wakeup();
    void HandleWakeup();
};