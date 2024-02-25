CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu11

%.o : %.c
	$(CC) $(CFLAGS) -c $<

mt_webserver : main.o webserver.o csapp.o
	$(CC) -o $@ main.o webserver.o csapp.o -lpthread

main.o : webserver.h csapp.h main.c

webserver.o : webserver.h csapp.h webserver.c

csapp.o : csapp.h csapp.c

clean :
	rm -f *.o mt_webserver
