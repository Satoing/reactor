#include "Connection.h"

Connection::Connection(EventLoop *loop, std::unique_ptr<Socket> clientsock)
           : loop_(loop), connsock_(std::move(clientsock)), isclosed_(false), connchannel_(new Channel(connsock_->fd(), loop_)) {
    connchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    connchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    connchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    connchannel_->setwritecallback(std::bind(&Connection::writecallback, this));

    connchannel_->useet();
    connchannel_->enablereading();
}

Connection::~Connection() {}

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
    
    if(outputbuffer_.size() == 0) {
        connchannel_->disablewriting();
        sendcompletecallback_(shared_from_this());
    }
}

void Connection::errorcallback() {
    connchannel_->remove();
    isclosed_ = true;
    errorcallback_(shared_from_this());
}

void Connection::closecallback() {
    connchannel_->remove();
    isclosed_ = true;
    closecallback_(shared_from_this());
}

void Connection::seterrorcallback(std::function<void(spConnection)> fn) {
    errorcallback_ = fn;
}

void Connection::setclosecallback(std::function<void(spConnection)> fn) {
    closecallback_ = fn;
}

void Connection::setmessagecallback(std::function<void(spConnection, std::string&)> fn) {
    messagecallback_ = fn;
}

void Connection::setsendcompletecallback(std::function<void(spConnection)> fn) {
    sendcompletecallback_ = fn;
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

                // 对接收到的数据的处理流程，采用回调函数
                messagecallback_(shared_from_this(), message);
            }
            break;
        }
        else if(readn == 0) {
            closecallback();
            break;
        }
    }
}

// 可能在工作线程中执行，也可能在IO线程中执行，需要判断当前线程是否是IO线程
void Connection::send(std::string &message) {
    if(isclosed_) return;

    if(loop_->isinloopthread()) {
        sendinloop(message);
    } else {
        loop_->queueinloop(std::bind(&Connection::sendinloop, this, message));
    }
}

void Connection::sendinloop(std::string &message) {
    outputbuffer_.appendwithhead(message.data(), message.size());
    connchannel_->enablewriting();
}