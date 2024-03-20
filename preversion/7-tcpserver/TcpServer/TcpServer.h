#pragma once

#include <string>
#include <functional>
#include <map>

#include "Acceptor.h"
#include "Connection.h"


class TcpServer {
private:
    EventLoop loop_;
    Acceptor *acceptor_;
    std::map<int, Connection*> conns_;

public:
    TcpServer(const std::string &ip, uint16_t port);
    ~TcpServer();
    void start();
    void newconnection(Socket *connsock);
    void closeconnection(Connection *conn);
    void errorconnection(Connection *conn);
};