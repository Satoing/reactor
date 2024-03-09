#pragma once

#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>

class Connection;
using spConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection> {
private:
    EventLoop *loop_;
    std::unique_ptr<Socket> connsock_;
    std::unique_ptr<Channel> connchannel_;
    std::function<void(spConnection)> errorcallback_;
    std::function<void(spConnection)> closecallback_;
    std::function<void(spConnection, std::string&)> messagecallback_;
    std::function<void(spConnection)> sendcompletecallback_;
    Buffer inputbuffer_;
    Buffer outputbuffer_;
    std::atomic_bool isclosed_;

public:
    Connection(EventLoop *loop, std::unique_ptr<Socket> clientsock);
    ~Connection();
    std::string ip() const;
    uint16_t port() const;
    int fd() const;

    void onmessage();
    void writecallback();
    void errorcallback();
    void closecallback();

    void seterrorcallback(std::function<void(spConnection)> fn);
    void setclosecallback(std::function<void(spConnection)> fn);
    void setmessagecallback(std::function<void(spConnection, std::string&)> fn);
    void setsendcompletecallback(std::function<void(spConnection)> fn);

    void send(const char *data, int size);
};