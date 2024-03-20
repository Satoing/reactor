#pragma once

#include "TcpServer.h"

class EchoServer {
private:
    TcpServer server_;
public:
    EchoServer(const std::string &ip, uint16_t port);
    ~EchoServer();

    void start();
    
    void HandleConnection(Connection *conn);
    void HandleClose(Connection *conn);
    void HandleError(Connection *conn);
    void HandleMessage(Connection *conn, std::string &message);
    void HandleSendComplete(Connection *conn);
    void HandleEpollTimeout(EventLoop *loop);
};