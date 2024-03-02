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

class Ctcpclient {
private:
    int m_clientfd;
    string m_ip;
    unsigned short m_port;
public:
    Ctcpclient(): m_clientfd(-1) {};

    bool connect(const string &ip, const unsigned short &port) {
        if(m_clientfd != -1) return false;
        m_ip = ip; m_port = port;

        if( (m_clientfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
            return false;
        }
        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(m_port);
        struct hostent *h;
        if( (h = gethostbyname(m_ip.c_str())) == nullptr) {
            ::close(m_clientfd); m_clientfd = -1;
            return false;
        }
        memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
        if( (::connect(m_clientfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) == -1) {
            ::close(m_clientfd); m_clientfd = -1;
            return false;
        }
        return true;
    }

    bool send(const string &buf) {
        if(m_clientfd == -1) return false;
        if(::send(m_clientfd, buf.data(), buf.size(), 0) <= 0)
            return false;
        return true;
    }

    bool send(void *buf, int size) {
        if(m_clientfd == -1) return false;
        if(::send(m_clientfd, buf, size, 0) <= 0)
            return false;
        return true;
    }

    bool sendfile(const string &filename, const int filesize) {
        ifstream fin(filename, ios::binary);
        if(fin.is_open() == false) {
            cout << "打开文件" << filename << "失败" << endl;
            return false;
        }
        int onread = 0; // 每次发送的字节数
        int total = 0; // 已经发送的字节数
        char buf[7]; // 缓冲区

        while(true) {
            memset(buf, 0, sizeof(buf));
            if(filesize-total > 7) onread = 7;
            else onread = filesize - total;

            fin.read(buf, onread);
            if(send(buf, onread) == false) return false;
            total += onread;

            if(total == filesize) break;
        }
        return true;
    }

    bool recv(string &buf, const int maxlen) {
        buf.clear();
        buf.resize(maxlen);
        int readn = ::recv(m_clientfd, &buf[0], buf.size(), 0);
        if(readn <= 0) {
            buf.clear(); return false;
        }
        buf.resize(buf.size());
        return true;
    }
    
    bool close() {
        if(m_clientfd == -1) return false;
        ::close(m_clientfd);
        m_clientfd = -1;
        return true;
    }

    ~Ctcpclient() {close();};
};

// ./client 127.0.0.1 5005 <filepath> <size>
int main(int argc, char *argv[]) {
    Ctcpclient client;
    if( client.connect(argv[1], atoi(argv[2])) == false) {
        perror("connect failed"); return -1;
    }

    /* 客户端发送文件的流程 */
    // 1. 把文件名和文件大小告诉服务端
    struct st_fileinfo {
        char name[256];
        int size;
    } fileinfo;
    memset(&fileinfo, 0, sizeof(fileinfo));
    strcpy(fileinfo.name, argv[3]);
    fileinfo.size = atoi(argv[4]);
    client.send(&fileinfo, sizeof(fileinfo));

    // 2. 等待服务端的确认
    string buf;
    if(client.recv(buf, 1024) == false) {
        perror("接收失败");
    }
    cout << "服务端返回：" << buf << endl;

    // 3. 发送文件内容
    if(client.sendfile(fileinfo.name, fileinfo.size) == false) {
        perror("发送失败");
    }

    // 4. 等待服务端确认  
    cout << "服务端返回：" << buf << endl;
}