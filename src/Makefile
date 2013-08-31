CFLAGS=-Wall -march=native -O2 -g -export-dynamic $(shell pkg-config --cflags gtk+-3.0 libtoxcore)
LDFLAGS=$(shell pkg-config --libs gtk+-3.0 libtoxcore)

all: main

main: gtkwindow.o configdir.o misc.o storage.o dht.o callbacks.o main.o

clean:
	rm -f main *.o

objclean:
	rm -f *.o
