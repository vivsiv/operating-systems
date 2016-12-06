CC=gcc

CFLAGS=-Wall -lmraa -lm -std=gnu99

GDBFLAGS=-g -Wall -pthread

all:
	$(CC) $(CFLAGS) lab4.c -o lab4

clean:
	rm lab4

check:

dist:
	tar pczf lab4-303652195.tar.gz lab4.c Makefile sample_log.txt README