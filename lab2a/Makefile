CC=gcc

CFLAGS=-Wall -pthread

GDBFLAGS=-g -Wall -pthread

# DEPS=cliserv.h

all:
	$(CC) $(CFLAGS) lab2_add.c -o lab2_add
	touch lab2_add.csv
	
clean:
	rm lab2_add
	rm lab2_add.csv

clean_charts:
	rm lab2_add-*

test:
	# Run your program for ranges of threads (2, 4, 8, 12) and iterations (100, 1000, 10000, 100000) 
	./lab2_add --iterations=100 --threads=2 >> lab2_add.csv
	./lab2_add --iterations=100 --threads=4 >> lab2_add.csv
	./lab2_add --iterations=100 --threads=8 >> lab2_add.csv
	./lab2_add --iterations=100 --threads=12 >> lab2_add.csv

	./lab2_add --iterations=1000 --threads=2 >> lab2_add.csv
	./lab2_add --iterations=1000 --threads=4 >> lab2_add.csv
	./lab2_add --iterations=1000 --threads=8 >> lab2_add.csv
	./lab2_add --iterations=1000 --threads=12 >> lab2_add.csv

	./lab2_add --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --iterations=10000 --threads=12 >> lab2_add.csv

	./lab2_add --iterations=100000 --threads=2 >> lab2_add.csv
	./lab2_add --iterations=100000 --threads=4 >> lab2_add.csv
	./lab2_add --iterations=100000 --threads=8 >> lab2_add.csv
	./lab2_add --iterations=100000 --threads=12 >> lab2_add.csv

	# Re-run your tests, with yields, for ranges of threads (2,4,8,12) and iterations (10, 20, 40, 80, 100, 1000, 10000, 100000)
	./lab2_add --yield --iterations=10 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=10 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=10 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=10 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=20 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=20 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=20 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=20 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=20 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=20 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=20 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=20 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=40 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=40 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=40 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=40 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=80 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=80 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=80 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=80 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=100 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=100 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=100 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=100 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=1000 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=1000 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=1000 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=1000 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=10000 --threads=12 >> lab2_add.csv

	./lab2_add --yield --iterations=100000 --threads=2 >> lab2_add.csv
	./lab2_add --yield --iterations=100000 --threads=4 >> lab2_add.csv
	./lab2_add --yield --iterations=100000 --threads=8 >> lab2_add.csv
	./lab2_add --yield --iterations=100000 --threads=12 >> lab2_add.csv

	# Compare the average execution time of the yield and non-yield versions a range threads (2, 8) 
	# and of iterations (100, 1000, 10000, 100000).
	# Already have all from part 1

	# For a single thread, graph the average cost per operation (non-yield) as a function of the number of iterations.
	./lab2_add --iterations=10 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=50 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=100 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=500 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=1000 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=5000 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=10000 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=50000 --threads=1 >> lab2_add.csv
	./lab2_add --iterations=100000 --threads=1 >> lab2_add.csv


	# Use your --yield option to confirm that, even for large numbers of threads (2, 4, 8, 12)
	# and iterations (10,000 for mutexes and CAS, only 1,000 for spin locks)
	./lab2_add --yield --sync=m --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --yield --sync=m --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --yield --sync=m --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --yield --sync=m --iterations=10000 --threads=12 >> lab2_add.csv

	./lab2_add --yield --sync=c --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --yield --sync=c --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --yield --sync=c --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --yield --sync=c --iterations=10000 --threads=12 >> lab2_add.csv

	./lab2_add --yield --sync=s --iterations=1000 --threads=2 >> lab2_add.csv
	./lab2_add --yield --sync=s --iterations=1000 --threads=4 >> lab2_add.csv
	./lab2_add --yield --sync=s --iterations=1000 --threads=8 >> lab2_add.csv
	./lab2_add --yield --sync=s --iterations=1000 --threads=12 >> lab2_add.csv

	# Using a large enough number of iterations (e.g. 10,000) to overcome the issues raised in the question 2.1.3, 
	# test all four (no yield) versions (unprotected, mutex, spin-lock, compare-and-swap) for a range of number of threads (1,2,4,8,12) and capture the output.  
	# Graph the average time per operation (non-yield), vs the number of threads.
	./lab2_add --iterations=100000 --threads=1 >> lab2_add.csv

	./lab2_add --sync=m --iterations=10000 --threads=1 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --sync=m --iterations=10000 --threads=12 >> lab2_add.csv

	./lab2_add --sync=c --iterations=10000 --threads=1 >> lab2_add.csv
	./lab2_add --sync=c --iterations=10000 --threads=2 >> lab2_add.csv
	./lab2_add --sync=c --iterations=10000 --threads=4 >> lab2_add.csv
	./lab2_add --sync=c --iterations=10000 --threads=8 >> lab2_add.csv
	./lab2_add --sync=c --iterations=10000 --threads=12 >> lab2_add.csv

	./lab2_add --sync=s --iterations=1000 --threads=1 >> lab2_add.csv
	./lab2_add --sync=s --iterations=1000 --threads=2 >> lab2_add.csv
	./lab2_add --sync=s --iterations=1000 --threads=4 >> lab2_add.csv
	./lab2_add --sync=s --iterations=1000 --threads=8 >> lab2_add.csv
	./lab2_add --sync=s --iterations=1000 --threads=12 >> lab2_add.csv



dist:
	tar pczf lab2a-303652195.tar.gz lab2_add.c Makefile README