CC=gcc

CFLAGS=-Wall -pthread -lmcrypt

GDBFLAGS=-g -Wall -pthread -lmcrypt

DEPS=cliserv.h

all:
	$(CC) $(CFLAGS) client.c -o client
	$(CC) $(CFLAGS) server.c -o server

client:
	$(CC) $(CFLAGS) client.c -o client

server:
	$(CC) $(CFLAGS) server.c -o server
	
clean:
	rm client
	rm server

dist:
	tar pczf lab1b-303652195.tar.gz cliserv.h server.c client.c Makefile README