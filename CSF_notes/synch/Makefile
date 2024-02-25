CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11 -O2

PROGS = incr_race incr_fixed life_genrand life_seq life_par

%.o : %.c
	$(CC) $(CFLAGS) -c $<

all : $(PROGS)

incr_race : incr_race.o
	$(CC) -o $@ incr_race.o -lpthread

incr_fixed : incr_fixed.o
	$(CC) -o $@ incr_fixed.o -lpthread

life_genrand : life_genrand.o life.o
	$(CC) -o $@ life_genrand.o life.o

life_seq : life_seq.o life.o
	$(CC) -o $@ life_seq.o life.o

life_par : life_par.o life.o bqueue.o
	$(CC) -o $@ life_par.o life.o bqueue.o -lpthread

incr_fixed : incr_fixed.o

incr_race.o : incr_race.c

incr_fixed.o : incr_fixed.c

life.o : life.c life.h

life_genrand.o : life_genrand.c life.h

life_seq.o : life_seq.c life.h

life_par.o : life_par.c life.h

bqueue.o : bqueue.c bqueue.h

clean :
	rm -f *.o $(PROGS)
