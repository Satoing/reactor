#pragma once

#include <string>
#include <functional>
#include <map>

#include "Acceptor.h"
#include "Connection.h"
#include <functional>


class TcpServer {
private:
    EventLoop loop_;
    Acceptor *acceptor_;
    std::map<int, Connection*> conns_;

    std::function<void(Connection*)> newconncallback_;
    std::function<void(Connection*)> closeconncallback_;
    std::function<void(Connection*)> errorconncallback_;
    std::function<void(Connection*, std::string&)> messagecallback_;
    std::function<void(Connection*)> sendcompletecallback_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;
public:
    TcpServer(const std::string &ip, uint16_t port);
    ~TcpServer();

    void start();
    
    void newconnection(Socket *connsock);
    void closeconnection(Connection *conn);
    void errorconnection(Connection *conn);
    void onmessage(Connection *conn, std::string &message);
    void sendcomplete(Connection *conn);
    void epolltimeout(EventLoop *loop);

    void setnewconncallback(std::function<void(Connection*)> fn);
    void setcloseconncallback(std::function<void(Connection*)> fn);
    void seterrorconncallback(std::function<void(Connection*)> fn);
    void setmessagecallback(std::function<void(Connection*, std::string&)> fn);
    void setsendcompletecallback(std::function<void(Connection*)> fn);
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);
};