Included files:
lab0.c - this contains the source code for the lab
Makefile - the Makefile ()
part_5_screencap.png - this is the screen cap for part 5 of the lab (running under --segfault and generating the backtrace)
part_6_screencap.png - this is the screen cap for part 6 of the lab (running under --segfault with a break point at line 78 to show the assignment of the null pointer)

Testing:
The first test runs the program with --input and --output specified, then compares the two files to check if they are the same (diff should be nothing)
The second test runs the program with --input and --output specified and --catch raised (should behave like the first test)
The third test runs the program with --segfault and --catch raised (should handle signal, print Segmentation Fault to the console, and exit with error code 3)

Sources:
I used this article: http://www.informit.com/articles/article.aspx?p=175771
to help me figure out how to parse long options.