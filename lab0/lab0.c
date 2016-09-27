#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


#define STDIN 0
#define STDOUT 1
#define BUF_SIZE 1024

void segfault_handler(int signal){
	fprintf(stderr, "Segmentation Fault\n");
	exit(3);
}

int main(int argc, char *argv[]) {
	int opt_char;

	static int segfault_flag;
	static int catch_flag;
	static struct option long_options[] = {
		{"input", required_argument, 0, 'i'},
		{"output", required_argument, 0, 'o'},
		{"segfault", no_argument, &segfault_flag, 1},
		{"catch", no_argument, &catch_flag, 1}
	};

	int ifd;
	int ofd;
	while ((opt_char = getopt_long(argc, argv, "i:o:", long_options, NULL)) != -1){
		switch (opt_char) {
			case 'i':
				//redirect --input argument to stdin fd
				ifd = open(optarg, O_RDONLY);
				if (ifd >= 0){
					close(STDIN);
					dup(ifd);
					close(ifd);
				}
				else {
					fprintf(stderr, "Could not open input file: %s\n", optarg);
					perror("Could not open input file");
					exit(1);
				}
				break;
				
			case 'o':
				//redirect --output argument to stdout fd
				ofd = creat(optarg, 0666);
				if (ofd >= 0){
					close(STDOUT);
					dup(ofd);
					close(ofd);
				}
				else {
					fprintf(stderr, "Could not create output file: %s\n", optarg);
					perror("Could not create output file");
					exit(2);
				}
				break;
				
			default:
				break;
		}
	}

	//Add segfault signal handler if --catch is specified
	if (catch_flag == 1){
		signal(SIGSEGV, segfault_handler);
	}

	//Cause a segfault if --segfault is specified
	int* null_pointer = NULL;
	if (segfault_flag == 1){	
		*null_pointer = 1;
	}

	char buf[BUF_SIZE];
	int n_bytes;
	n_bytes = read(STDIN, buf, BUF_SIZE);
	while (n_bytes > 0){
		write(STDOUT, buf, n_bytes);
		n_bytes = read(STDIN, buf, BUF_SIZE);
	}
    exit(0);
}