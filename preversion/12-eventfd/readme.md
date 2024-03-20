## 异步唤醒事件循环

之前代码存在的问题：

1. 将要发送的内容存放到缓冲区中的操作`Connection::send`在WORK线程中
2. 真正发送缓冲区中数据的操作`Connection::writecallback()`在IO线程中

两种线程都要操作发送缓冲区，就会存在竞争。最直观的方法是加锁，但是当并发量很大的时候效率很低。

解决方案：把要发送的内容放到缓冲区的操作也交给IO线程完成。

1. 每个IO线程（从EventLoop）都设置一个任务队列
2. 工作线程执行完任务后，把发送的操作添加到IO线程的任务队列中
3. 使用eventfd的方式唤醒IO线程（把eventfd加入epoll中），让它执行任务队列中的任务