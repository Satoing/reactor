#include "TcpServer.h"

int main() {
    TcpServer server("127.0.0.1", 5005);
    server.start();
}