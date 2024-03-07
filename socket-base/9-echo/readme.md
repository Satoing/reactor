## 通知机制

使用回调函数实现：

1. 数据发送完毕通知tcpserver
2. epoll_wait超时通知tcpserver

需要注意这里数据发送完毕是指本次读完缓冲区中的内容再处理后的发送完毕，可能没有接收完客户端发来的所有消息。

## 实现echo server

TcpServer要管理Acceptor和Connection，还是属于比较底层的类。所以将业务完全从TcpServer中分离开。

创建EchoServer类，在TcpServer基础上进一步封装业务代码。TcpServer使用回调函数的方式调用EchoServer中的业务代码。
