#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock): loop_(loop), connsock_(clientsock) {
    connchannel_ = new Channel(connsock_->fd(), loop_);
    connchannel_->setcallback(std::bind(&Channel::onmessage, connchannel_));
    connchannel_->useet();
    connchannel_->enablereading();
}

Connection::~Connection() {
    delete connsock_;
    delete connchannel_;
}