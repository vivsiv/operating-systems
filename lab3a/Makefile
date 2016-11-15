CC=gcc

CFLAGS=-Wall

GDBFLAGS=-g -Wall -pthread

all:
	$(CC) $(CFLAGS) lab3a.c -o lab3a

clean:
	rm lab3a
	rm *.csv

dist:
	tar pczf lab3a-303652195.tar.gz lab3a.c lab3a.h Makefile README