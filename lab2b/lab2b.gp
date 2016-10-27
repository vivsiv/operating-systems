#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded add project
#
# input: lab2_add.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # add operations
#	5. run time (ns)
#	6. run time per operation (ns)
#	7. total sum at end of run (should be zero)
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2b_1.png ... threads and iterations that run (unprotected) w/o failure
#	lab2_add-2.png ... cost per operation of yielding
#	lab2_add-3.png ... cost per operation vs number of iterations
#	lab2_add-4.png ... threads and iterations that run (protected) w/o failure
#	lab2_add-5.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

set title "Add: throughput vs number of threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "throughput ops/s"
set logscale y 10
set output 'lab2b_1.png'
set key right top
# grep out only successful (sum=0) yield runs
plot \
     "< grep 'add-m' lab2_add.csv" using ($2):(1000000000/$6) title 'add mutex' with linespoints lc rgb 'red', \
     "< grep 'add-s' lab2_add.csv" using ($2):(1000000000/$6) title 'add spin' with linespoints lc rgb 'green', \
     "< grep 'list-none-m' lab2_list.csv" using ($2):(1000000000/$7) title 'list mutex' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s' lab2_list.csv" using ($2):(1000000000/$7) title 'list spin' with linespoints lc rgb 'orange'