import os
import sys
from subprocess import call

if len(sys.argv) < 2:
	print "Usage: python check.py <correct csvs folder>"

curr_dir = os.getcwd();
test_dir = curr_dir + "/" + sys.argv[1]

for filename in os.listdir(test_dir):
	# print filename
	test_file = test_dir + "/" + filename
	curr_file = curr_dir + "/" + filename

	print "Checking ... {0}".format(filename)
	call(["diff", curr_file, test_file])
	print "Finished ... {0}".format(filename)