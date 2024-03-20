#include "Socket.h"

Socket::Socket(int fd):fd_(fd) {}

Socket::~Socket() {
    // ::close(fd_);
}

int Socket::fd() const {
    return fd_;
}

void Socket::setreuseaddr(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::settcpnodelay(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setreuseport(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setkeepalive(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::bind(InetAddress &servaddr) {
    if(::bind(fd_, servaddr.addr(), sizeof(sockaddr)) < 0) exit(-1);
}

void Socket::listen(int qn) {
    if(::listen(fd_, qn) < 0) exit(-1);
}

int Socket::accept(InetAddress &clientaddr) {
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int connfd = accept4(fd_, (struct sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);
    clientaddr.setaddr(peeraddr);
    return connfd;
}
