#pragma once

#include <string>
#include <functional>
#include <map>

#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

class TcpServer {
private:
    EventLoop *mainloop_;              // 主事件循环
    std::vector<EventLoop*> subloops_; // 从事件循环

    Acceptor *acceptor_;
    std::map<int, Connection*> conns_;

    ThreadPool *pool_;
    int threadnum_;                    // 线程池中的线程数

    std::function<void(Connection*)> newconncallback_;
    std::function<void(Connection*)> closeconncallback_;
    std::function<void(Connection*)> errorconncallback_;
    std::function<void(Connection*, std::string&)> messagecallback_;
    std::function<void(Connection*)> sendcompletecallback_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;
public:
    TcpServer(const std::string &ip, uint16_t port, int threadnum);
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