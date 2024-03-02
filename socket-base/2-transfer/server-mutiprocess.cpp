#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
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

    bool recv(void *buf, int size) {
        if(m_connfd == -1) return false;

        if(::recv(m_connfd, buf, size, 0) <= 0) {
            return false;
        }
        return true;
    }

    bool recvfile(const string &filename, const int filesize) {
        if(m_connfd == -1) return false;
        ofstream fout;
        fout.open(filename, ios::binary);
        if(fout.is_open() == false) {
            return false;
        }

        int onread = 0; // 每次接收的字节数
        int total = 0; // 已经接收的字节数
        char buf[7]; // 缓冲区

        while(true) {
            memset(buf, 0, sizeof(buf));
            if(filesize-total > 7) onread = 7;
            else onread = filesize - total;

            if(recv(buf, onread) == false) return false;
            fout.write(buf, onread);
            total += onread;

            if(total == filesize) break;
        }
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
        int pid = fork();
        if(pid != 0) {
            serv.closeclient();
            continue;
        }
        serv.closeserv();
        /* 服务端接收文件的流程 */
        // 1. 获取文件名和文件大小
        struct st_fileinfo {
            char name[256];
            int size;
        } fileinfo;
        memset(&fileinfo, 0, sizeof(fileinfo));
        if(serv.recv(&fileinfo, sizeof(fileinfo)) == false) {
            return -1;
        }
        cout << "filename: " << fileinfo.name << "\nfilesize: " 
             << fileinfo.size << endl;

        // 2. 向客户端发送确认
        if(serv.send("ok") == false) {
            perror("发送失败");
        }

        // 3. 接收文件内容
        if(serv.recvfile(string("temp/")+fileinfo.name, fileinfo.size) == false) {
            perror("文件接收失败");
        }

        // 4. 向客户端发送确认
        if(serv.send("ok") == false) {
            perror("发送失败");
        }

        serv.closeclient();
        cout << "重新等待客户端连接" << endl;
    }
}