BINARIES = flip

CC = gcc
CFLAGS = -Wall -g -c
LDLIBS = -lrt -lm -lpthread

all: $(BINARIES)

clean:
	rm -f *.o $(BINARIES)

flip: flip.o 
flip.o: flip.c flip.h uint128.h