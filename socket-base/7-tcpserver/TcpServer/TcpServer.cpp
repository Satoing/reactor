#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, uint16_t port) {
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconncallback(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {
    delete acceptor_;
}

void TcpServer::start() {
    loop_.run();
}

void TcpServer::newconnection(Socket *connsock) {
    Connection *connection = new Connection(&loop_, connsock);
}