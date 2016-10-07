CC=gcc

CFLAGS=-Wall -pthread

GDBFLAGS=-g -Wall -pthread

all:
	$(CC) $(CFLAGS) client.c -o client
	$(CC) $(CFLAGS) server.c -o server
	
clean:
	rm client
	rm server

dist:
	tar pczf lab1a-303652195.tar.gz server.c client.c Makefile README