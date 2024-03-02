all: server client server-mutiprocess

server: server.cpp
	g++ -o server server.cpp

client: client.cpp
	g++ -o client client.cpp

server-mutiprocess: server-mutiprocess.cpp
	g++ -o server-mutiprocess server-mutiprocess.cpp

clean:
	rm client server server-mutiprocess