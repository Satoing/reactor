#pragma once

#include <string>
#include <functional>
#include <map>
#include <memory>

#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

class TcpServer {
private:
    std::unique_ptr<EventLoop> mainloop_;              // 主事件循环
    std::vector<std::unique_ptr<EventLoop>> subloops_; // 从事件循环

    Acceptor acceptor_;
    std::map<int, spConnection> conns_;

    int threadnum_;                    // 线程池中的线程数
    ThreadPool pool_;

    std::function<void(spConnection)> newconncallback_;
    std::function<void(spConnection)> closeconncallback_;
    std::function<void(spConnection)> errorconncallback_;
    std::function<void(spConnection, std::string&)> messagecallback_;
    std::function<void(spConnection)> sendcompletecallback_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;
public:
    TcpServer(const std::string &ip, uint16_t port, int threadnum);
    ~TcpServer();

    void start();
    
    void newconnection(std::unique_ptr<Socket> connsock);
    void closeconnection(spConnection conn);
    void errorconnection(spConnection conn);
    void onmessage(spConnection conn, std::string &message);
    void sendcomplete(spConnection conn);
    void epolltimeout(EventLoop *loop);

    void setnewconncallback(std::function<void(spConnection)> fn);
    void setcloseconncallback(std::function<void(spConnection)> fn);
    void seterrorconncallback(std::function<void(spConnection)> fn);
    void setmessagecallback(std::function<void(spConnection, std::string&)> fn);
    void setsendcompletecallback(std::function<void(spConnection)> fn);
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);
};