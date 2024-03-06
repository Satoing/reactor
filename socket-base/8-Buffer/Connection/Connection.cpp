#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock): loop_(loop), connsock_(clientsock) {
    connchannel_ = new Channel(connsock_->fd(), loop_);

    connchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    connchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    connchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    connchannel_->setwritecallback(std::bind(&Connection::writecallback, this));

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

void Connection::writecallback() {
    int writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if(writen > 0) outputbuffer_.erase(0, writen);
    
    if(outputbuffer_.size() == 0) connchannel_->disablewriting();
}

void Connection::errorcallback() {
    errorcallback_(this);
}

void Connection::closecallback() {
    closecallback_(this);
}

void Connection::seterrorcallback(std::function<void(Connection*)> fn) {
    errorcallback_ = fn;
}

void Connection::setclosecallback(std::function<void(Connection*)> fn) {
    closecallback_ = fn;
}

void Connection::setmessagecallback(std::function<void(Connection*, std::string&)> fn) {
    messagecallback_ = fn;
}

void Connection::onmessage() {
    char buf[1024] = {0};
    
    while(true) {
        int readn = ::recv(fd(), buf, sizeof(buf), 0);
        if(readn > 0) {
            inputbuffer_.append(buf, readn);
            memset(buf, 0, sizeof(buf));
        }
        else if(readn == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            while(true) {
                int len;
                memcpy(&len, inputbuffer_.data(), 4);
                if(inputbuffer_.size() < len+4) break;

                std::string message(inputbuffer_.data()+4, len);
                inputbuffer_.erase(0, len+4);
                std::cout << "收到数据：" << message.data() << std::endl;

                // 对接收到的数据的处理流程，采用回调函数
                messagecallback_(this, message);
            }
            break;
        }
        else if(readn == 0) {
            closecallback();
            break;
        }
    }
}

void Connection::send(const char *data, int size) {
    outputbuffer_.append(data, size);
    connchannel_->enablewriting();
}