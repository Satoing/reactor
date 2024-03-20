#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, uint16_t port) {
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconncallback(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
    loop_.setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
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
    connection->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));
    connection->setmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    
    conns_[connection->fd()] = connection;

    if(newconncallback_) newconncallback_(connection);
}

void TcpServer::closeconnection(Connection *conn) {
    if(closeconncallback_) closeconncallback_(conn);
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection *conn) {
    if(errorconncallback_) errorconncallback_(conn);
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection *conn, std::string &message) {
    if(messagecallback_) messagecallback_(conn, message);
}

void TcpServer::sendcomplete(Connection *conn) {
    if(sendcompletecallback_) sendcompletecallback_(conn);
}

void TcpServer::epolltimeout(EventLoop *loop) {
    if(epolltimeoutcallback_) epolltimeoutcallback_(loop);
}

void TcpServer::setnewconncallback(std::function<void(Connection*)> fn) {
    newconncallback_ = fn;
}

void TcpServer::setcloseconncallback(std::function<void(Connection*)> fn) {
    closeconncallback_ = fn;
}

void TcpServer::seterrorconncallback(std::function<void(Connection*)> fn) {
    errorconncallback_ = fn;
}
void TcpServer::setmessagecallback(std::function<void(Connection*, std::string&)> fn) {
    messagecallback_ = fn;
}

void TcpServer::setsendcompletecallback(std::function<void(Connection*)> fn) {
    sendcompletecallback_ = fn;
}

void TcpServer::setepolltimeoutcallback(std::function<void(EventLoop*)> fn) {
    epolltimeoutcallback_ = fn;
}