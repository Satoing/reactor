RELYFILES = InetAddress/InetAddress.cpp Socket/Socket.cpp Epoll/Epoll.cpp Channel/Channel.cpp EventLoop/EventLoop.cpp TcpServer/TcpServer.cpp Acceptor/Acceptor.cpp Connection/Connection.cpp Buffer/Buffer.cpp EchoServer/EchoServer.cpp
INCLUDEPATH = -IInetAddress -ISocket -IEpoll -IChannel -IEventLoop -ITcpServer -IAcceptor -IConnection -IBuffer -IEchoServer

all: server client

server: server.cpp ${RELYFILES}
	g++ -o server server.cpp ${RELYFILES} ${INCLUDEPATH}

client: client.cpp
	g++ -o client client.cpp

clean: 
	rm client server