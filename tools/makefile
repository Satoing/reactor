all: libpublic.a libpublic.so

libpublic.a: hello.h hello.cpp
	g++ -c hello.cpp -o libpublic.a

libpublic.so: hello.h hello.cpp
	g++ -fPIC -shared hello.cpp -o libpublic.so

clean: 
	rm -f libpublic.a libpublic.so