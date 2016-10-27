CC=gcc

CFLAGS=-Wall -pthread

PROF_FLAGS=-g -Wall -pthread -lprofiler

GDBFLAGS=-g -Wall -pthread

build:
	$(CC) $(CFLAGS) lab2_list.c SortedList.c -o lab2_list
	$(CC) $(CFLAGS) lab2_add.c -o lab2_add

build_list:
	$(CC) $(CFLAGS) lab2_list.c SortedList.c -o lab2_list

build_add:
	$(CC) $(CFLAGS) lab2_add.c -o lab2_add

tests:
	touch lab2_add.csv
	touch lab2_list.csv

	# Mutex synchronized adds, 10,000 iterations, 1,2,4,8,12 threads
	./lab2_add --sync=m --iterations=10000 --threads=1 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=12 >> lab2_add.csv

	# Spin-lock synchronized adds, 10,000 iterations, 1,2,4,8,12 threads
	./lab2_add --sync=s --iterations=10000 --threads=1 >> lab2_add.csv
	./lab2_add --sync=s --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --sync=s --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --sync=s --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --sync=s --iterations=10000 --threads=12 >> lab2_add.csv

	# Mutex synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads
	./lab2_list --sync=m --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=2 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=12 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=24 --iterations=1000 >> lab2_list.csv

	# Spin-lock synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads
	./lab2_list --sync=s --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=2 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=12 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=24 --iterations=1000 >> lab2_list.csv


	# Build your program with debug symbols run the spin-lock list test (1,000 iterations 12 threads) under the profiler, and analyze the results to determine where the cycles are being spent.


profile:

graphs:
	./gnuplot lab2b.gp

tarball:
	tar pczf lab2b-303652195.tar.gz lab2_list.c SortedList.h SortedList.c Makefile README

clean:
	rm lab2_list
	rm lab2_list.csv
	rm lab2_add
	rm lab2_add.csv

clean_add:
	rm lab2_add
	rm lab2_add.csv

clean_list:
	rm lab2_list
	rm lab2_list.csv