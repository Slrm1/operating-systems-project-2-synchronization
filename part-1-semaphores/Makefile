CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
LDLIBS = -pthread

all: shm_proc

shm_proc: shm_processes.c
	$(CC) $(CFLAGS) shm_processes.c -o shm_proc $(LDLIBS)

clean:
	rm -f shm_proc

.PHONY: all clean
