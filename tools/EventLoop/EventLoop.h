#pragma once

#include "Epoll.h"
#include "Connection.h"
#include <functional>
#include <memory>
#include <queue>
#include <mutex>
#include <map>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

class Epoll;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;

class EventLoop {
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;
    pid_t tid_;                                 // 存放事件循环所在的线程id，在run函数中赋值
    std::queue<std::function<void()>> tasks_;   // 执行sendinloop的任务队列
    std::mutex mutex_;
    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_;      // 将wakeupfd加入事件循环中
    int timerfd_;                               // 定时器的fd
    std::unique_ptr<Channel> timerchannel_;
    bool ismainloop_;
    // 实现定时删除空闲connection
    std::mutex mmutex_;
    std::map<int, spConnection> conns_;         // 存放运行在该事件循环上的所有connection对象
    std::function<void(int)> removeconncallback_;
    int timetvl_;
    int timeout_;

public:
    EventLoop(bool ismainloop, int timetvl = 30, int timeout = 80);
    ~EventLoop();
    
    void run();
    void removechannel(Channel *ch);
    void updateChannel(Channel *ch);  // 使用联合类型中的指针，将Channel类添加到红黑树上 
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);

    bool isinloopthread();            // 判断事件循环是否在IO线程中，否则在工作线程中
    void queueinloop(std::function<void()> fn);
    void wakeup();
    void HandleWakeup();
    
    void HandleTimer();

    // 闹钟时间到了，遍历所有connection对象，判断是否超时
    void newconnection(spConnection conn);
    void setremoveconncallback(std::function<void(int)> fn);
    void removeconn(int fd);
};