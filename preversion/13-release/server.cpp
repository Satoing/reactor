#include "EchoServer.h"

int main() {
    EchoServer server("127.0.0.1", 5005, 3, 3);
    server.start();
}