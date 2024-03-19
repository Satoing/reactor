#include "EventLoop.h"

int createtimerfd(int sec = 5) {
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    itimerspec timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &timeout, 0);
    return tfd;
}

EventLoop::EventLoop(bool ismainloop, int timetvl, int timeout)
          :ep_(new Epoll), wakeupfd_(eventfd(0, EFD_NONBLOCK)), wakechannel_(new Channel(wakeupfd_, this)), 
           timerfd_(createtimerfd(timetvl)), timerchannel_(new Channel(timerfd_, this)), 
           ismainloop_(ismainloop), timeout_(timeout), timetvl_(timetvl) {
    wakechannel_->enablereading();
    wakechannel_->setreadcallback(std::bind(&EventLoop::HandleWakeup, this));
    timerchannel_->enablereading();
    timerchannel_->setreadcallback(std::bind(&EventLoop::HandleTimer, this));
}

EventLoop::~EventLoop() {}

void EventLoop::run() {
    tid_ = syscall(SYS_gettid);
    while(true) {
        std::vector<Channel*> chs = ep_->loop(); 
        if(chs.size() == 0) epolltimeoutcallback_(this);
        for(auto ch : chs) ch->handleevent();
    }
}

void EventLoop::removechannel(Channel *ch) {
    if(ch->inepoll()) {
        if(epoll_ctl(ep_->epfd(), EPOLL_CTL_DEL, ch->fd(), 0) == -1) 
            exit(-1);
    }
}

void EventLoop::updateChannel(Channel *ch) {
    epoll_event event;
    event.data.ptr = ch;
    event.events = ch->events();
    
    if(ch->inepoll()) {
        if(epoll_ctl(ep_->epfd(), EPOLL_CTL_MOD, ch->fd(), &event) == -1) exit(-1);
    } else {
        if(epoll_ctl(ep_->epfd(), EPOLL_CTL_ADD, ch->fd(), &event) == -1) exit(-1);
    }
    ch->setinepoll();
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn) {
    epolltimeoutcallback_ = fn;
}

bool EventLoop::isinloopthread() {
    return tid_ == syscall(SYS_gettid);
}

void EventLoop::queueinloop(std::function<void()> fn) {
    // 添加到任务队列中
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push(fn);
    }
    // 唤醒事件循环
    wakeup();
}

// 随便往eventfd里面写点内容即可唤醒
void EventLoop::wakeup() {
    uint64_t i = 1;
    write(wakeupfd_, &i, sizeof(i));
}

void EventLoop::HandleWakeup() {
    uint64_t i;
    read(wakeupfd_, &i, sizeof(i));

    std::function<void()> fn;
    // 加锁不能放到循环里面
    std::lock_guard<std::mutex> lock(mutex_);

    while(!tasks_.empty()) {
        fn = std::move(tasks_.front());
        tasks_.pop();
        fn();
    }
}

void EventLoop::HandleTimer() {
    // 重新计时
    itimerspec timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.it_value.tv_sec = timetvl_;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_, 0, &timeout, 0);
    
    if(!ismainloop_) {
        std::cout << "EventLoop::HandleTimer() thread is " << syscall(SYS_gettid) << " fd ";
        time_t now = time(0);
        // 注意循环时删除元素的坑！
        for(auto it = conns_.begin(); it != conns_.end();) {
            std::cout << it->first << " ";
            if(it->second->timeout(now, timeout_)) {
                removeconncallback_(it->first);
                std::lock_guard<std::mutex> lock(mmutex_);
                conns_.erase(it++);
            } else it++;
        }
        std::cout << '\n';
    }
}

void EventLoop::newconnection(spConnection conn) {
    std::lock_guard<std::mutex> lock(mmutex_);
    conns_[conn->fd()] = conn;
}

void EventLoop::setremoveconncallback(std::function<void(int)> fn) {
    removeconncallback_ = fn;
}