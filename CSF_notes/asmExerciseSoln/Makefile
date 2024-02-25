CC = gcc
CFLAGS = -g -no-pie
LDFLAGS = -no-pie

EXES = hello arrayMax

%.o  : %.S
	$(CC) -c $(CFLAGS) -o $*.o $*.S

all : $(EXES)

hello : hello.o
	$(CC) $(LDFLAGS) -o $@ hello.o

arrayMax : arrayMax.o
	$(CC) $(LDFLAGS) -o $@ arrayMax.o

clean :
	rm -f $(EXES) *.o
