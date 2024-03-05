#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, uint16_t port) {
    acceptor_ = new Acceptor(&loop_, ip, port);
}

TcpServer::~TcpServer() {
    delete acceptor_;
}

void TcpServer::start() {
    loop_.run();
}