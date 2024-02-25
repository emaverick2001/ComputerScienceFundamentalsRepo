CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu99 -fPIC

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

all : fnptr myprog instr.so

fnptr : fnptr.o
	$(CC) -o $@ fnptr.o

myprog : myprog.o
	$(CC) -o $@ myprog.o

instr.so : instr.o
	$(CC) -shared -nostdlib -o $@ instr.o -ldl

clean :
	rm -f *.o fnptr myprog instr.so
