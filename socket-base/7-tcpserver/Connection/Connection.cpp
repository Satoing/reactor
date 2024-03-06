#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock): loop_(loop), connsock_(clientsock) {
    connchannel_ = new Channel(connsock_->fd(), loop_);

    connchannel_->setreadcallback(std::bind(&Channel::onmessage, connchannel_));
    connchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    connchannel_->setclosecallback(std::bind(&Connection::closecallback, this));

    connchannel_->useet();
    connchannel_->enablereading();
}

Connection::~Connection() {
    delete connsock_;
    delete connchannel_;
}

std::string Connection::ip() const {
    return connsock_->ip();
}

uint16_t Connection::port() const {
    return connsock_->port();
}

int Connection::fd() const {
    return connsock_->fd();
}

void Connection::errorcallback() {
    // std::cout << "连接出现错误" << std::endl;
    // close(fd());
    errorcallback_(this);
}

void Connection::closecallback() {
    // std::cout << "客户端断开连接" << std::endl;
    // close(fd());
    closecallback_(this);
}

void Connection::seterrorcallback(std::function<void(Connection*)> fn) {
    errorcallback_ = fn;
}

void Connection::setclosecallback(std::function<void(Connection*)> fn) {
    closecallback_ = fn;
}