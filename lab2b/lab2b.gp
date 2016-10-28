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
#   2. number of lists
#	3. # threads
#	4. # iterations per thread
#	5. # lists
#	6. # operations performed (threads x iterations x (ins + lookup + delete))
#	7. run time (ns)
#	8. run time per operation (ns)
#   9. Avg wait for lock time (only mutex)
#   
#
# output:
#	lab2b_1.png ... add and list: throughput vs number of threads
#	lab2b_2.png ... mutexes: wait for lock time and avg operation time vs num threads 
#	lab2b_3.png ... cost per operation vs number of iterations
#	lab2b_4.png ... threads and iterations that run (protected) w/o failure
#	lab2b_5.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

set title "Throughput vs Number of Threads"
set xlabel "Num Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (ops/s)"
set logscale y 10
set output 'lab2b_1.png'
set key right top
# grep out only successful (sum=0) yield runs
plot \
     "< grep 'add-m,.*,10000,' lab2_add.csv" using ($2):(1000000000/$6) title 'add mutex' with linespoints lc rgb 'red', \
     "< grep 'add-s,.*,10000,' lab2_add.csv" using ($2):(1000000000/$6) title 'add spin' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,1,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title 'list mutex' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,1,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title 'list spin' with linespoints lc rgb 'orange'


set title "Mutex: Avg Wait for Lock Time, Avg Time Per Operation vs Number of Threads"
set xlabel "Num Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Avg Time/Op (s)"
set logscale y 10
set output 'lab2b_2.png'
set key right top
# grep out only successful (sum=0) yield runs
plot \
     "< grep 'list-none-m,1,.*,1000' lab2_list.csv" using ($3):($8) title 'time per operation' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,1,.*,1000' lab2_list.csv" using ($3):($9) title 'wait for lock' with linespoints lc rgb 'blue'


set title "Lists: Unprotected Threads and Iterations that run without failure"
set xlabel "Num Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep list-id-none,4, lab2_list.csv" using ($3):($4) title 'unprotected' with points lc rgb 'red', \
     "< grep list-id-m,4, lab2_list.csv" using ($3):($4) title 'mutex' with points lc rgb 'green', \
     "< grep list-id-s,4, lab2_list.csv" using ($3):($4) title 'spin' with points lc rgb 'blue'


set title "Multiple Lists, Mutex: Throughput vs Lists"
set xlabel "Num Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (ops/s)"
set logscale y 10
set output 'lab2b_4.png'
set key right top
# grep out only successful (sum=0) yield runs
plot \
     "< grep 'list-none-m,1,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '1 lists' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,4,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '4 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,8,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '8 lists' with linespoints lc rgb 'violet', \
     "< grep 'list-none-m,16,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '16 lists' with linespoints lc rgb 'blue'


set title "Multiple Lists, Spin Lock: Throughput vs Lists"
set xlabel "Num Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (ops/s)"
set logscale y 10
set output 'lab2b_5.png'
set key right top
# grep out only successful (sum=0) yield runs
plot \
     "< grep 'list-none-s,1,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '1 lists' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,4,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '4 lists' with linespoints lc rgb 'green', \
      "< grep 'list-none-s,8,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '8 lists' with linespoints lc rgb 'violet', \
     "< grep 'list-none-s,16,.*,1000,' lab2_list.csv" using ($3):(1000000000/$8) title '16 lists' with linespoints lc rgb 'blue'