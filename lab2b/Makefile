CC=gcc

CFLAGS=-g -Wall -pthread

build:
	$(CC) $(CFLAGS) lab2_list.c SortedList.c -o lab2_list
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


	# Run the list mutex test again for 1,000 iterations and 1, 2, 4, 8, 16, 24 threads, and 
	# plot the wait-for-lock time, and the average time per operation against the number of competing threads.


	# Run your program with --yield=id, 4 lists, 1,4,8,12,16 threads, and 1, 2, 4, 8, 16 iterations (and no synchronization) to 
	# see how many iterations it takes to reliably fail.
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=1 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=2 --iterations=1 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=1 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=1 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=1 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=1 >> lab2_list.csv

	./lab2_list --yield=id --lists=4 --threads=1 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=2 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=2 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=2 >> lab2_list.csv

	./lab2_list --yield=id --lists=4 --threads=1 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=2 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=4 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=4 >> lab2_list.csv

	./lab2_list --yield=id --lists=4 --threads=1 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=2 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=8 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=8 >> lab2_list.csv

	./lab2_list --yield=id --lists=4 --threads=1 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=2 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab2_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab2_list.csv


	# Run your program with --yield=id, 4 lists, 1,4,8,12,16 threads, and 10, 20, 40, 80 iterations  --sync=s and --sync=m 
	./lab2_list --sync=m --yield=id --lists=4 --threads=1 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=2 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=4 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=8 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=12 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=16 --iterations=10 >> lab2_list.csv

	./lab2_list --sync=m --yield=id --lists=4 --threads=1 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=2 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=4 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=8 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=12 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=16 --iterations=20 >> lab2_list.csv

	./lab2_list --sync=m --yield=id --lists=4 --threads=1 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=2 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=4 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=8 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=12 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=16 --iterations=40 >> lab2_list.csv

	./lab2_list --sync=m --yield=id --lists=4 --threads=1 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=2 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=4 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=8 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=12 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --threads=16 --iterations=80 >> lab2_list.csv

	./lab2_list --sync=s --yield=id --lists=4 --threads=1 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=2 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=4 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=8 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=12 --iterations=10 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=16 --iterations=10 >> lab2_list.csv

	./lab2_list --sync=s --yield=id --lists=4 --threads=1 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=2 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=4 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=8 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=12 --iterations=20 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=16 --iterations=20 >> lab2_list.csv

	./lab2_list --sync=s --yield=id --lists=4 --threads=1 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=2 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=4 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=8 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=12 --iterations=40 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=16 --iterations=40 >> lab2_list.csv

	./lab2_list --sync=s --yield=id --lists=4 --threads=1 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=2 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=4 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=8 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=12 --iterations=80 >> lab2_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --threads=16 --iterations=80 >> lab2_list.csv
	# Graph these results (as you did last week) and include the results as lab2b_3.png.

	# Rerun both synchronized versions without yields for 1000 iterations, 1,2,4,8,12 threads, and 1,4,8,16 lists.  
	# For each synchronization mechanism, graph the aggregated throughput 
	# (total operations per second, as you did for lab2a_1.png) vs the number of threads, with a separate line for each number of lists.  Call these graphs lab2b_4.png and lab2b_5.png
	# ./lab2_list --sync=m --threads=1 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=m --threads=2 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=m --threads=4 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=m --threads=8 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=m --threads=12 --lists=1 --iterations=1000 >> lab2_list.csv

	./lab2_list --sync=m --threads=1 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=2 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=4 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=8 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=12 --lists=4 --iterations=1000 >> lab2_list.csv

	./lab2_list --sync=m --threads=1 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=2 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=4 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=8 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=12 --lists=8 --iterations=1000 >> lab2_list.csv

	./lab2_list --sync=m --threads=1 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=2 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=4 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=8 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=m --threads=12 --lists=16 --iterations=1000 >> lab2_list.csv

	# ./lab2_list --sync=s --threads=1 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=s --threads=2 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=s --threads=4 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=s --threads=8 --lists=1 --iterations=1000 >> lab2_list.csv
	# ./lab2_list --sync=s --threads=12 --lists=1 --iterations=1000 >> lab2_list.csv

	./lab2_list --sync=s --threads=1 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=2 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=4 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=8 --lists=4 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=12 --lists=4 --iterations=1000 >> lab2_list.csv

	./lab2_list --sync=s --threads=1 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=2 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=4 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=8 --lists=8 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=12 --lists=8 --iterations=1000 >> lab2_list.csv

	./lab2_list --sync=s --threads=1 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=2 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=4 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=8 --lists=16 --iterations=1000 >> lab2_list.csv
	./lab2_list --sync=s --threads=12 --lists=16 --iterations=1000 >> lab2_list.csv

profile:
	# Build your program with debug symbols run the spin-lock list test (1,000 iterations 12 threads) under the profiler, and analyze the results to determine where the cycles are being spent.
	LD_PRELOAD=/u/cs/grad/vivek/gperftools/lib/libprofiler.so CPUPROFILE=/u/cs/grad/vivek/Development/cs_111/lab2b/lab2_list.prof /u/cs/grad/vivek/Development/cs_111/lab2b/lab2_list --sync=s --iterations=1000 --threads=12
	/u/cs/grad/vivek/gperftools/bin/pprof --text /u/cs/grad/vivek/Development/cs_111/lab2b/lab2_list /u/cs/grad/vivek/Development/cs_111/lab2b/lab2_list.prof

graphs:
	/u/cs/grad/zhou/iloveos/gnuplot lab2b.gp

tarball:
	tar pczf lab2b-303652195.tar.gz lab2_add.c lab2_list.c SortedList.h SortedList.c *.png Makefile README

clean:
	rm lab2_list
	rm lab2_list.csv
	rm lab2_add
	rm lab2_add.csv
