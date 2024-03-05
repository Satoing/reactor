#pragma once

#include <arpa/inet.h>
#include <string>

class InetAddress {
private:
    sockaddr_in addr_;
public:
    InetAddress() = default;
    InetAddress(const std::string &ip, uint16_t port);  // 服务端地址构造函数，传入ip为点分十进制
    InetAddress(const sockaddr_in &addr);               // 客户端地址构造函数
    ~InetAddress() {};

    const char* ip() const;        // 获取ip
    uint16_t port() const;         // 获取端口
    const sockaddr* addr() const;  // bind的时候需要sockaddr地址
    void setaddr(const sockaddr_in &addr);
};