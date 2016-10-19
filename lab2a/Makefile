CC=gcc

CFLAGS=-Wall -pthread

GDBFLAGS=-g -Wall -pthread

all:
	$(CC) $(CFLAGS) lab2_add.c -o lab2_add
	touch lab2_add.csv
	$(CC) $(CFLAGS) lab2_list.c SortedList.c -o lab2_list
	touch lab2_list.csv

add:
	$(CC) $(CFLAGS) lab2_add.c -o lab2_add
	touch lab2_add.csv

list:
	$(CC) $(CFLAGS) lab2_list.c SortedList.c -o lab2_list
	touch lab2_list.csv
	
clean:
	rm lab2_add
	rm lab2_add.csv
	rm lab2_list
	rm lab2_list.csv

clean_add:
	rm lab2_add
	rm lab2_add.csv

clean_list:
	rm lab2_list
	rm lab2_list.csv

clean_add_charts:
	rm lab2_add-*

clean_list_charts:
	rm lab2_list-*

test_add:
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

test_list:
	# Run your program with a single thread, and increasing numbers of iterations (10, 100, 1000, 10000, 20000), 
	# Graph the time per operation vs the number of iterations (for --threads=1).  
	./lab2_list --threads=1 --iterations=10 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=10000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=20000 >> lab2_list.csv

	# Run your program and see how many parallel threads (2,4,8,12) and iterations (10,100,1000) it takes to fairly consistently demonstrate a problem.  
	./lab2_list --threads=2 --iterations=10 >> lab2_list.csv
	./lab2_list --threads=4 --iterations=10 >> lab2_list.csv
	./lab2_list --threads=8 --iterations=10 >> lab2_list.csv
	./lab2_list --threads=12 --iterations=10 >> lab2_list.csv

	./lab2_list --threads=2 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=4 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=8 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=12 --iterations=100 >> lab2_list.csv

	./lab2_list --threads=2 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 >> lab2_list.csv

	# Then run it again using various combinations of yield options and see how many threads (2,4,8,12) and iterations (2,4,8,16,32)
	# conflicts between inserts (--yield=i)
	# conflicts between deletes (--yield=d)
	# conflicts between inserts and lookups (--yield=il)
	# conflicts between deletes and lookups (--yield=dl)
	./lab2_list --yield=i --threads=2 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=i --threads=4 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=i --threads=8 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=i --threads=12 --iterations=2 >> lab2_list.csv

	./lab2_list --yield=d --threads=2 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=d --threads=4 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=d --threads=8 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=d --threads=12 --iterations=2 >> lab2_list.csv

	./lab2_list --yield=il --threads=2 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=il --threads=4 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=il --threads=8 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=il --threads=12 --iterations=2 >> lab2_list.csv

	./lab2_list --yield=dl --threads=2 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=dl --threads=4 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=dl --threads=8 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=dl --threads=12 --iterations=2 >> lab2_list.csv

	./lab2_list --yield=i --threads=2 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=i --threads=4 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=i --threads=8 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=i --threads=12 --iterations=4 >> lab2_list.csv

	./lab2_list --yield=d --threads=2 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=d --threads=4 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=d --threads=8 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=d --threads=12 --iterations=4 >> lab2_list.csv

	./lab2_list --yield=il --threads=2 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=il --threads=4 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=il --threads=8 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=il --threads=12 --iterations=4 >> lab2_list.csv

	./lab2_list --yield=dl --threads=2 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=dl --threads=4 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=dl --threads=8 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=dl --threads=12 --iterations=4 >> lab2_list.csv


	./lab2_list --yield=i --threads=2 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=i --threads=4 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=i --threads=8 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=i --threads=12 --iterations=8 >> lab2_list.csv

	./lab2_list --yield=d --threads=2 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=d --threads=4 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=d --threads=8 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=d --threads=12 --iterations=8 >> lab2_list.csv

	./lab2_list --yield=il --threads=2 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=il --threads=4 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=il --threads=8 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=il --threads=12 --iterations=8 >> lab2_list.csv

	./lab2_list --yield=dl --threads=2 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=dl --threads=4 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=dl --threads=8 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=dl --threads=12 --iterations=8 >> lab2_list.csv

	./lab2_list --yield=i --threads=2 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=i --threads=4 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=i --threads=8 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=i --threads=12 --iterations=16 >> lab2_list.csv

	./lab2_list --yield=d --threads=2 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=d --threads=4 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=d --threads=8 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=d --threads=12 --iterations=16 >> lab2_list.csv

	./lab2_list --yield=il --threads=2 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=il --threads=4 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=il --threads=8 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=il --threads=12 --iterations=16 >> lab2_list.csv

	./lab2_list --yield=dl --threads=2 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=dl --threads=4 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=dl --threads=8 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=dl --threads=12 --iterations=16 >> lab2_list.csv

	./lab2_list --yield=i --threads=2 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=i --threads=4 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=i --threads=8 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=i --threads=12 --iterations=32 >> lab2_list.csv

	./lab2_list --yield=d --threads=2 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=d --threads=4 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=d --threads=8 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=d --threads=12 --iterations=32 >> lab2_list.csv

	./lab2_list --yield=il --threads=2 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=il --threads=4 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=il --threads=8 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=il --threads=12 --iterations=32 >> lab2_list.csv

	./lab2_list --yield=dl --threads=2 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=dl --threads=4 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=dl --threads=8 --iterations=32 >> lab2_list.csv
	./lab2_list --yield=dl --threads=12 --iterations=32 >> lab2_list.csv


	# (--sync=m) and (--sync=s).  Using your --yield options, demonstrate these protections seems eliminate all problems, even for large numbers of threads (12) and iterations (32).
	./lab2_list --sync=m --yield=i --threads=12 --iterations=32 >> lab2_list.csv
	./lab2_list --sync=m --yield=d --threads=12 --iterations=32 >> lab2_list.csv
	./lab2_list --sync=m --yield=il --threads=12 --iterations=32 >> lab2_list.csv
	./lab2_list --sync=m --yield=dl --threads=12 --iterations=32 >> lab2_list.csv

	./lab2_list --sync=s --yield=i --threads=12 --iterations=32 >> lab2_list.csv
	./lab2_list --sync=s --yield=d --threads=12 --iterations=32 >> lab2_list.csv
	./lab2_list --sync=s --yield=il --threads=12 --iterations=32 >> lab2_list.csv
	./lab2_list --sync=s --yield=dl --threads=12 --iterations=32 >> lab2_list.csv


	# Choose an appropriate number of iterations (e.g. 1000) and rerun your program without the yields. 
	# Run the protected methods for a wide range of numbers of threads (1, 2, 4, 8, 12, 16, 24).  
	# Graph the (corrected) per operation times (for each of the three synchronization options: unprotected, mutex, spin)
	./lab2_list --sync=m --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=2 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=12 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=24 --iterations=1000 >> lab2_list.csv

	./lab2_list --sync=s --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=2 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=12 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=24 --iterations=1000 >> lab2_list.csv


dist:
	tar pczf lab2a-303652195.tar.gz lab2_add.c lab2_list.c SortedList.h SortedList.c lab2_add-* lab2_list-* Makefile README