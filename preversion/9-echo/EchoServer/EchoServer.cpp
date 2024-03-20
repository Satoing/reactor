#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, uint16_t port): server_(ip, port) {
    server_.setnewconncallback(std::bind(&EchoServer::HandleConnection, this, std::placeholders::_1));
    server_.setcloseconncallback(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    server_.seterrorconncallback(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    server_.setsendcompletecallback(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    server_.setmessagecallback(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    server_.setepolltimeoutcallback(std::bind(&EchoServer::HandleEpollTimeout, this, std::placeholders::_1));
}

EchoServer::~EchoServer() {

}

void EchoServer::start() {
    server_.start();
}

void EchoServer::HandleConnection(Connection *conn) {
    std::cout << "connect client:" << conn->ip() << ":" << conn->port() 
                << ", fd:" << conn->fd() << std::endl;
}

void EchoServer::HandleClose(Connection *conn) {
    std::cout << "客户端断开连接" << std::endl;
}

void EchoServer::HandleError(Connection *conn) {
    std::cout << "连接出现错误" << std::endl;
}

void EchoServer::HandleMessage(Connection *conn, std::string &message) {
    message = message + "（已阅）";
    int len = message.size();
    conn->send(message.data(), len+4);
}

void EchoServer::HandleSendComplete(Connection *conn) {
    std::cout << "send complete" << std::endl;
}

void EchoServer::HandleEpollTimeout(EventLoop *loop) {
    std::cout << "epoll timeout" << std::endl;
}