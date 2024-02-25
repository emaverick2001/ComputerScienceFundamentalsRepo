# CSF Assignment 3 Makefile

CC = gcc
CFLAGS = -std=gnu11 -pedantic -Wall -Wall -Wextra


csim: csim.o
	$(CC) -o csim csim.o

csim.o: csim.c
	$(CC) $(CFLAGS) -c csim.c

clean:
	rm -f *.o csim
