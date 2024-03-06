#include "Socket.h"

int createnonblocking() {
    int listenfd;
    if( (listenfd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
        return -1;
    }
    return listenfd;
}

Socket::Socket(int fd):fd_(fd) {}

Socket::~Socket() {
    ::close(fd_);
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
    ip_ = servaddr.ip();
    port_ = servaddr.port();
}

void Socket::listen(int qn) {
    if(::listen(fd_, qn) < 0) exit(-1);
}

int Socket::accept(InetAddress &clientaddr) {
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int connfd = accept4(fd_, (struct sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);
    clientaddr.setaddr(peeraddr);

    ip_ = clientaddr.ip();
    port_ = clientaddr.port();

    return connfd;
}

std::string Socket::ip() const {
    return ip_;
}

uint16_t Socket::port() const {
    return port_;
}