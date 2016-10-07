#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static struct termios old_term_settings;
static int port_no;
static char *log_file;
static int encrypt_flag;

static int sock_fd;

#define BUF_SIZE 1024
#define HOST_NAME "localhost"

void error(char *msg){
	perror(msg);
	exit(1);
}

//Set the terminal to non-canonical, non-echo mode
void setup_terminal(){
	if (tcgetattr(STDIN_FILENO, &old_term_settings) == -1) {
		perror("tcgetattr");
		exit(1);
	}
	
	struct termios new_term_settings;
	new_term_settings = old_term_settings;
	new_term_settings.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term_settings) == -1) {
		perror("tcsetattr");
		exit(1);
	}
}

void cleanup_terminal(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term_settings) == -1) {
			perror("tcsetattr");
		}
}

void parse_options(int argc, char *argv[]){
	int opt_char;

	struct option long_options[] = {
		{"port", required_argument, 0, 'p'},
		{"log", required_argument, 0, 'l'},
		{"encrypt", no_argument, &encrypt_flag, 1}
	};

	while ((opt_char = getopt_long(argc, argv, "p:l:", long_options, NULL)) != -1){
		switch (opt_char){
			case 'p':
				port_no = atoi(optarg);
				fprintf(stdout, "Got port number: %d from options\n", port_no);
				//sockfd = socket(AF_INET, SOCK_STREAM, 0);
				break;
			case 'l':
				log_file = optarg;
				fprintf(stdout, "Got log file: %s from options\n", log_file);
				//log_fd = open(log_file, O_WRONLY);
				break;
			default:
				break;
		}
	}
}

void *read_from_socket(void *args){
	char buf[BUF_SIZE];
	int n_bytes;

	while ((n_bytes = read(sock_fd, buf, BUF_SIZE)) > 0){
		for (int i = 0; i < n_bytes; i++){
			char c_out = buf[i];
			switch (c_out) {
				//Map <cr> or <lf>
				case '\r':
				case '\n':
					write(STDOUT_FILENO, "\r\n", 2);
					break;
				//Handle ^D
				case '\004':
					close(sock_fd);
					// if (log_fd >= 0) close(log_fd);
					exit(0);
				//If not a special case 
				default:
					//Write the character out to STDOUT in the kb process
					write(STDOUT_FILENO, buf + i, 1);
					break;
			}
		}
	}

	exit(1);

	return NULL;
}

int main(int argc, char *argv[]){
	atexit(cleanup_terminal);

	setup_terminal();

	parse_options(argc, argv);

	struct sockaddr_in serv_addr;
	struct hostent *server;

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		error("Error opening socket");
	}

	server = gethostbyname(HOST_NAME);
	if (server == NULL){
		fprintf(stderr, "ERROR: no such host %s\n", HOST_NAME);
		exit(0);
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port_no);

	fprintf(stdout, "Connecting to server\n");
	if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		error("Error connecting to host");
	}

	// int log_fd = -1;
	// if (log_file != NULL){
	// 	log_fd = open(log_file, O_WRONLY);
	// }
	// if (log_fd < 0){
	// 	fprintf(stderr, "ERROR: unable to open log file: %s\n", log_file);
	// }

	//spin up a second thread to read from the socket and write responses back to STDOUT
	pthread_t read_socket_thread;
	pthread_create(&read_socket_thread, NULL, read_from_socket, NULL);

	//in the main thread we read from the keyboard and write to the socket
	char buf[BUF_SIZE];
	int n_bytes;
	while ((n_bytes = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
		for (int i = 0; i < n_bytes; i++){
			char c_out = buf[i];
			switch (c_out) {
				//Map <cr> or <lf>
				case '\r':
				case '\n':
					write(sock_fd, "\n", 1);
					write(STDOUT_FILENO, "\r\n", 2);
					break;
				//Handle ^D
				case '\004':
					pthread_cancel(read_socket_thread);
					close(sock_fd);
					// if (log_fd >= 0) close(log_fd);
					exit(0);
				default:
					//Write the character out to STDOUT in the kb process
					write(sock_fd, buf + i, 1);
					write(STDOUT_FILENO, buf + i, 1);
					break;
			}
		}
	}
	exit(0);

}