CC=gcc

CFLAGS=-g -Wall

all:
	$(CC) $(CFLAGS) lab0.c -o lab0

check:
	touch input.txt
	echo this is a test file > input.txt
	./lab0 --input=input.txt --output=output.txt
	diff input.txt output.txt
	./lab0 --input=input.txt --output=output.txt --catch
	diff input.txt output.txt
	./lab0 --segfault --catch
	
clean:
	rm lab0 input.txt output.txt

dist:
	tar pczf lab0-303652195.tar.gz lab0.c Makefile README.txt part_5_screencap.png part_6_screencap.png