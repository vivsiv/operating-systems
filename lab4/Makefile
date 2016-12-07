CC=gcc

CFLAGS=-Wall -lmraa -lm -std=gnu99

GDBFLAGS=-g -Wall -pthread

all:
	$(CC) $(CFLAGS) part1.c -o part1
	$(CC) $(CFLAGS) part2.c -o part2

clean:
	rm part1
	rm part2

check:

dist:
	tar pczf lab4-303652195.tar.gz part1.c part2.c part1_log.txt part2_log.txt Makefile README