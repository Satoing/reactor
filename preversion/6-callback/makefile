RELYFILES = InetAddress/InetAddress.cpp Socket/Socket.cpp Epoll/Epoll.cpp Channel/Channel.cpp EventLoop/EventLoop.cpp
INCLUDEPATH = -IInetAddress -ISocket -IEpoll -IChannel -IEventLoop

all: server client

server: server.cpp ${RELYFILES}
	g++ -o server server.cpp ${RELYFILES} ${INCLUDEPATH}

client: client.cpp
	g++ -o client client.cpp

clean: 
	rm client server