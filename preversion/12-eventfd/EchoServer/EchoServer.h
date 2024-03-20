#pragma once

#include "TcpServer.h"

class EchoServer {
private:
    TcpServer server_;
    ThreadPool pool_;
public:
    EchoServer(const std::string &ip, uint16_t port, int subthreadnum, int workthreadnum);
    ~EchoServer();

    void start();
    
    void HandleConnection(spConnection conn);
    void HandleClose(spConnection conn);
    void HandleError(spConnection conn);
    void HandleMessage(spConnection conn, std::string &message);
    void HandleSendComplete(spConnection conn);
    void HandleEpollTimeout(EventLoop *loop);

    void onmessage(spConnection conn, std::string &message);
};