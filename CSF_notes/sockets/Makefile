CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu99 -fPIC

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

all : server arithserver arithclient

server : server.o
	$(CC) -o $@ server.o

arithserver : arithserver.o csapp.o
	$(CC) -o $@ arithserver.o csapp.o -lpthread

arithclient : arithclient.o csapp.o
	$(CC) -o $@ arithclient.o csapp.o -lpthread

clean :
	rm -f *.o server arithserver arithclient
