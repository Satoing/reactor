## 使用回调函数

1. 使用std::bind统一服务端和客户端事件函数的参数
2. 调用std::function实现回调函数

## 封装事件循环

定义EventLoop类，完全隐藏server中的epoll细节。