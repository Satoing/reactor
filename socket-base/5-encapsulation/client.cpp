#include <iostream>
#include <string>
#include <cstring>
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

int main(int argc, char *argv[]) {
    Ctcpclient client;
    if( client.connect(argv[1], atoi(argv[2])) == false) {
        perror("connect failed"); return -1;
    }

    string buf;
    for(int i = 0; i < 3; i++) {
        buf = "这是第"+to_string(i+1)+"个消息";

        if(client.send(buf) == false) {
            cout << "send failed." << endl;
        }
        cout << "发送内容：" << buf << endl;
        if(client.recv(buf, 1024) == false) {
            cout << "recv failed." << endl;
        }
        cout << "接收内容：" << buf << endl;
        sleep(1);
    }
    client.close();
}