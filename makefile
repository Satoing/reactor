INCLUDEDIR = -Itools
LIBDIR = -Ltools
SRCDIR = app
INCLUDEFILES = -lpublic

all: main

main: ${SRCDIR}/main.cpp
	g++ ${SRCDIR}/main.cpp -o main ${LIBDIR} ${INCLUDEFILES} ${INCLUDEDIR}

clean:
	rm -f main