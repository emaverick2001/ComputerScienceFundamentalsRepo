CC = gcc
CFLAGS = -g -no-pie

# Executables to build
EXES = hello

% : %.S
	$(CC) $(CFLAGS) $*.S -o $@

all : $(EXES)

clean :
	rm -f $(EXES)
