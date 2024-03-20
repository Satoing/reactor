实现封装：

1. 把网络地址协议封装成`InetAdderss`类
2. 把socket库函数封装成`Socket`类
3. 把epoll的各种操作封装成`Epoll`类
4. 把TCP连接通道封装成`Channel`类：使用event.data.ptr封装更多内容