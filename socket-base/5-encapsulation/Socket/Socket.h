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

class Socket {
private:
    const int fd_;           // 只在构造的时候传入
public:
    Socket(int fd);
    ~Socket();

    int fd() const;
    void setreuseaddr(bool on);
    void settcpnodelay(bool on);
    void setreuseport(bool on);
    void setkeepalive(bool on);
    void bind(InetAddress &servaddr);
    void listen(int qn = 128);
    int accept(InetAddress &clientaddr);
};