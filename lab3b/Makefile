run:
	python lab3b.py super.csv group.csv bitmap.csv inode.csv directory.csv indirect.csv

clean:
	rm *.txt

check:
	sort lab3b_check.txt > sorted_test.txt
	sort lab3b_check_real.txt > sorted_real.txt
	diff sorted_test.txt sorted_real.txt
	rm sorted_test.txt
	rm sorted_real.txt

dist:
	tar pczf lab3b-303652195.tar.gz lab3b.py Makefile README