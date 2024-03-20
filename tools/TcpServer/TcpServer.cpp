#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, uint16_t port, int threadnum)
          :threadnum_(threadnum), mainloop_(new EventLoop(true, 30, 80)), acceptor_(mainloop_.get(), ip, port), pool_(threadnum_, "IO") {
    
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
    acceptor_.setnewconncallback(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));

    for(int i = 0; i < threadnum_; i++) {
        subloops_.emplace_back(new EventLoop(false, 30, 80));
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
        subloops_[i]->setremoveconncallback(std::bind(&TcpServer::removeconn, this, std::placeholders::_1));
        pool_.addtask(std::bind(&EventLoop::run, subloops_[i].get()));
    }
}

// 释放所有堆区资源，因为使用智能指针和栈对象，所以无需手动释放
TcpServer::~TcpServer() {}

void TcpServer::start() {
    mainloop_->run();
}

void TcpServer::newconnection(std::unique_ptr<Socket> connsock) {
    // 指定connection对应的从事件循环
    // 分配到对应事件循环采用算法：fd%threadnum_
    spConnection connection(new Connection(subloops_[connsock->fd()%threadnum_].get(), std::move(connsock)));
    connection->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    connection->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    connection->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));
    connection->setmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        conns_[connection->fd()] = connection;
    }
    subloops_[connection->fd()%threadnum_]->newconnection(connection);

    if(newconncallback_) newconncallback_(connection);
}

void TcpServer::closeconnection(spConnection conn) {
    if(closeconncallback_) closeconncallback_(conn);
    subloops_[conn->fd()%threadnum_]->removeconn(conn->fd());
    {
        std::lock_guard<std::mutex> lock(mutex_);
        conns_.erase(conn->fd());
    }
}

void TcpServer::errorconnection(spConnection conn) {
    if(errorconncallback_) errorconncallback_(conn);
    std::lock_guard<std::mutex> lock(mutex_);
    conns_.erase(conn->fd());
}

void TcpServer::onmessage(spConnection conn, std::string &message) {
    if(messagecallback_) messagecallback_(conn, message);
}

void TcpServer::sendcomplete(spConnection conn) {
    if(sendcompletecallback_) sendcompletecallback_(conn);
}

void TcpServer::epolltimeout(EventLoop *loop) {
    if(epolltimeoutcallback_) epolltimeoutcallback_(loop);
}

void TcpServer::removeconn(int fd) {
    std::lock_guard<std::mutex> lock(mutex_);
    conns_.erase(fd);
}

void TcpServer::setnewconncallback(std::function<void(spConnection)> fn) {
    newconncallback_ = fn;
}

void TcpServer::setcloseconncallback(std::function<void(spConnection)> fn) {
    closeconncallback_ = fn;
}

void TcpServer::seterrorconncallback(std::function<void(spConnection)> fn) {
    errorconncallback_ = fn;
}
void TcpServer::setmessagecallback(std::function<void(spConnection, std::string&)> fn) {
    messagecallback_ = fn;
}

void TcpServer::setsendcompletecallback(std::function<void(spConnection)> fn) {
    sendcompletecallback_ = fn;
}

void TcpServer::setepolltimeoutcallback(std::function<void(EventLoop*)> fn) {
    epolltimeoutcallback_ = fn;
}