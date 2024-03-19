#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <unistd.h>
#include <syscall.h>

class ThreadPool {
private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> taskqueue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic_bool stop_;
    std::string type_;  // 线程的类型："IO", "WORK"两种取值
public:
    ThreadPool(int num, const std::string &type);   // 启动num个工作线程
    ~ThreadPool();                                  // 需要等待工作线程执行完毕
    void addtask(std::function<void()> task);       // 向任务队列中加入可调用对象
    int size();
};