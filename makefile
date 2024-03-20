RELYFILES = tools/InetAddress/InetAddress.cpp tools/Socket/Socket.cpp tools/Epoll/Epoll.cpp tools/Channel/Channel.cpp tools/EventLoop/EventLoop.cpp tools/TcpServer/TcpServer.cpp tools/Acceptor/Acceptor.cpp tools/Connection/Connection.cpp tools/Buffer/Buffer.cpp tools/EchoServer/EchoServer.cpp tools/ThreadPool/ThreadPool.cpp tools/TimeStamp/TimeStamp.cpp
INCLUDEPATH = -Itools/InetAddress -Itools/Socket -Itools/Epoll -Itools/Channel -Itools/EventLoop -Itools/TcpServer -Itools/Acceptor -Itools/Connection -Itools/Buffer -Itools/EchoServer -Itools/ThreadPool -Itools/TimeStamp

all: server client

server: app/server.cpp ${RELYFILES}
	g++ -o bin/server app/server.cpp ${RELYFILES} ${INCLUDEPATH} -lpthread

client: app/client.cpp
	g++ -o bin/client app/client.cpp

clean: 
	rm bin/client bin/server