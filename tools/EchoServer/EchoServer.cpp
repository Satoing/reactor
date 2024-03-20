#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, uint16_t port, int threadnum, int workthreadnum):
                       server_(ip, port, threadnum), pool_(workthreadnum, "WORK") {
    server_.setnewconncallback(std::bind(&EchoServer::HandleConnection, this, std::placeholders::_1));
    server_.setcloseconncallback(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    server_.seterrorconncallback(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    server_.setsendcompletecallback(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    server_.setmessagecallback(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    server_.setepolltimeoutcallback(std::bind(&EchoServer::HandleEpollTimeout, this, std::placeholders::_1));
}

EchoServer::~EchoServer() {}

void EchoServer::start() {
    server_.start();
}

void EchoServer::HandleConnection(spConnection conn) {
    // 在主线程中执行
    // std::cout << "EchoServer Connection thread: " << syscall(SYS_gettid) << std::endl;
    std::cout << TimeStamp::now().tostring() << " new connection:" << conn->ip() << ":" << conn->port() 
                << ", fd:" << conn->fd() << std::endl;
}

void EchoServer::HandleClose(spConnection conn) {
    std::cout << TimeStamp::now().tostring() << " connection closed: " 
              << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleError(spConnection conn) {
    std::cout << "连接出现错误" << std::endl;
}

void EchoServer::HandleMessage(spConnection conn, std::string &message) {
    // 没有工作线程，直接由IO线程处理。
    if(pool_.size() == 0) onmessage(conn, message);
    // 否则将业务添加到工作线程中，避免从事件循环阻塞
    else pool_.addtask(std::bind(&EchoServer::onmessage, this, conn, message));
}

void EchoServer::onmessage(spConnection conn, std::string &message) {
    // std::cout << TimeStamp::now().tostring() << " recv fd" << conn->fd() 
    //           << "'s message: " << message << std::endl;
    message = message + "（已阅）";
    int len = message.size();
    conn->send(message);
}

void EchoServer::HandleSendComplete(spConnection conn) {
    // std::cout << "send complete" << std::endl;
}

void EchoServer::HandleEpollTimeout(EventLoop *loop) {
    std::cout << "epoll timeout" << std::endl;
}