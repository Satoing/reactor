## 进一步封装

1. 将server封装成TcpServer类。主程序中使用构造函数创建server，调用start启动server。

2. 将Channel进一步封装为Acceptor类和Connection类。一个server有一个Acceptor成员和多个Connection成员。