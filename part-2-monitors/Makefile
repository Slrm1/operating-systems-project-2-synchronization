CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -I.
LDLIBS = -pthread

all: main

main: BENSCHILLIBOWL.o main.o
	$(CC) $(CFLAGS) BENSCHILLIBOWL.o main.o -o main $(LDLIBS)

BENSCHILLIBOWL.o: BENSCHILLIBOWL.c BENSCHILLIBOWL.h
	$(CC) $(CFLAGS) -c BENSCHILLIBOWL.c

main.o: main.c BENSCHILLIBOWL.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f main *.o

.PHONY: all clean
