#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, uint16_t port) {
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconncallback(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {
    delete acceptor_;
    for(auto &conn : conns_) {
        delete conn.second;
    }
}

void TcpServer::start() {
    loop_.run();
}

void TcpServer::newconnection(Socket *connsock) {
    Connection *connection = new Connection(&loop_, connsock);
    connection->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    connection->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    connection->setmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    
    conns_[connection->fd()] = connection;
    std::cout << "connect client:" << connection->ip() << ":" << connection->port() 
                << ", fd:" << connection->fd() << std::endl;
}

void TcpServer::closeconnection(Connection *conn) {
    std::cout << "客户端断开连接" << std::endl;
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection *conn) {
    std::cout << "连接出现错误" << std::endl;
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection *conn, std::string &message) {
    message = message + "（已阅）";
    int len = message.size();
    // 将二进制写入string中
    std::string tempbuf((char*)&len, 4);
    tempbuf.append(message);
    // ::send(conn->fd(), tempbuf.data(), len+4, 0);
    conn->send(tempbuf.data(), len+4);
}