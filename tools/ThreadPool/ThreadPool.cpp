#include "ThreadPool.h"

ThreadPool::ThreadPool(int num, const std::string &type):stop_(false), type_(type) {
    for(int i = 0; i < num; i++) {
        // 使用emplace_back避免调用拷贝构造函数
        // 函数对象使用lambda函数
        threads_.emplace_back([this] {
            // std::cout << "create thread: " << syscall(SYS_gettid) << std::endl;
            while(stop_ == false) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    condition_.wait(lock, [this] {
                        return (stop_ == true) || (!taskqueue_.empty());
                    });
                    // 如果要设置了stop_=true且任务队列为空了，结束线程
                    if(stop_ == true && taskqueue_.empty()) return;

                    task = std::move(taskqueue_.front());
                    taskqueue_.pop();
                }
                // std::cout << type_ << " thread start task: " << syscall(SYS_gettid) << std::endl;
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop_ = true;
    condition_.notify_all();

    for(std::thread &th : threads_) th.join();
}

void ThreadPool::addtask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }
    condition_.notify_one();
}

int ThreadPool::size() {
    return threads_.size();
}