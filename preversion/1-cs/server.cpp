#include <iostream>
#include <string>
#include <cstring>
using namespace std;

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Ctcpserver {
private:
    int m_listenfd;
    int m_connfd;
    string m_clientip;
    unsigned short m_port;
public:
    Ctcpserver(): m_listenfd(-1), m_connfd(-1) {};

    bool init(unsigned short port) {
        if(m_listenfd != -1) return false;
        m_port = port;

        if( (m_listenfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
            return false;
        }

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(m_port);
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(m_listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
            ::close(m_listenfd); m_listenfd = -1;
            return false;
        }

        // 监听队列的典型长度是5
        if(listen(m_listenfd, 5) == -1) {
            ::close(m_listenfd); m_listenfd = -1;
            return false;
        }
        return true;
    }

    bool accept() {
        if(m_connfd != -1) return false;
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);
        if( (m_connfd = ::accept(m_listenfd, (struct sockaddr*)&clientaddr, &len)) == -1) {
            return false;
        }
        m_clientip = inet_ntoa(clientaddr.sin_addr);
        return true;
    }

    const string& clientip() {
        return m_clientip;
    }

    bool send(const string &buf) {
        if(m_connfd == -1) return false;
        if(::send(m_connfd, buf.data(), buf.size(), 0) <= 0) {
            return false;
        }
        return true;
    }

    bool recv(string &buf, const int maxlen) {
        if(m_connfd == -1) return false;
        buf.clear();
        buf.resize(maxlen);
        int readn = ::recv(m_connfd, &buf[0], buf.size(), 0);
        if(readn <= 0) {
            buf.clear(); return false;
        }
        buf.resize(buf.size());
        return true;
    }

    bool closeserv() {
        if(m_listenfd == -1) return false;
        ::close(m_listenfd);
        m_listenfd = -1;
        return true;
    }

    bool closeclient() {
        if(m_connfd == -1) return false;
        ::close(m_connfd);
        m_connfd = -1;
        return true;
    }

    ~Ctcpserver() {closeserv(); closeclient();};
};

int main() {
    Ctcpserver serv;
    if(serv.init(5005) == false) {
        perror("init failed.");
    }
    cout << "等待客户端连接" << endl;
    while(serv.accept() == true) {
        cout << "客户端 " << serv.clientip() << "已连接" << endl;
        string buf;
        while(true) {
            if(serv.recv(buf, 1024) == false) {
                break;
            }
            cout << "接收内容：" << buf << endl;
            serv.send("ok");
            cout << "发送ok" << endl;
        }
        serv.closeclient();
    }
}