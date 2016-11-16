import os
from subprocess import call

curr_dir = os.getcwd();
test_dir = curr_dir + "/test_csvs"

for filename in os.listdir(test_dir):
	# print filename
	test_file = test_dir + "/" + filename
	curr_file = curr_dir + "/" + filename

	print "Checking ... {0}".format(filename)
	call(["diff", curr_file, test_file])
	print "Finished ... {0}".format(filename)