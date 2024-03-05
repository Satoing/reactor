#pragma once

#include <string>
#include <functional>

#include "Acceptor.h"
#include "Connection.h"


class TcpServer {
private:
    EventLoop loop_;
    Acceptor *acceptor_;
public:
    TcpServer(const std::string &ip, uint16_t port);
    ~TcpServer();
    void start();
    void newconnection(Socket *connsock);
};