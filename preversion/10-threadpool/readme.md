## 使用线程池

1. 实现多线程的主从I/O模型：主线程运行主事件循环，处理与新客户端的连接；子线程运行从事件循环，处理与客户端的消息收发；
2. 增加工作线程：从事件循环中包括IO和服务器内部对业务的处理，后者可能阻塞，从而导致整个从事件循环阻塞。所以增加工作线程单独处理业务。
