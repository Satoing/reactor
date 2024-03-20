#pragma once

#include "TcpServer.h"

class EchoServer {
private:
    TcpServer server_;
    ThreadPool pool_;
public:
    EchoServer(const std::string &ip, uint16_t port, int subthreadnum = 3, int workthreadnum = 5);
    ~EchoServer();

    void start();
    
    void HandleConnection(Connection *conn);
    void HandleClose(Connection *conn);
    void HandleError(Connection *conn);
    void HandleMessage(Connection *conn, std::string &message);
    void HandleSendComplete(Connection *conn);
    void HandleEpollTimeout(EventLoop *loop);

    void onmessage(Connection *conn, std::string &message);
};