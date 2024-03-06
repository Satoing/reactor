#pragma once

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "InetAddress.h"

int createnonblocking();

class Socket {
private:
    const int fd_;           // 只在构造的时候传入
    std::string ip_;         // 监听socket ip为服务端ip，连接socket ip为对端ip
    uint16_t port_;
public:
    Socket(int fd);
    ~Socket();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;
    void setipport(const std::string ip, uint16_t port);

    void setreuseaddr(bool on);
    void settcpnodelay(bool on);
    void setreuseport(bool on);
    void setkeepalive(bool on);
    void bind(InetAddress &servaddr);
    void listen(int qn = 128);
    int accept(InetAddress &clientaddr);
};