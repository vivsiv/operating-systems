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
#include <fcntl.h>
#include <mcrypt.h>

#define BUF_SIZE 256
#define SEED 14

static int port_no;
static int encrypt_flag = 0;

static MCRYPT td;

void error(char *msg){
	perror(msg);
	exit(1);
}

void parse_options(int argc, char *argv[]){
	int opt_char;

	struct option long_options[] = {
		{"port", required_argument, 0, 'p'},
		{"encrypt", no_argument, &encrypt_flag, 1}
	};

	while ((opt_char = getopt_long(argc, argv, "p:l:", long_options, NULL)) != -1){
		switch (opt_char){
			case 'p':
				port_no = atoi(optarg);
				fprintf(stdout, "Got port number: %d from options\n", port_no);
				break;
			default:
				break;
		}
	}
}

void setup_encryption(){
	int key_size = 16; //128 bits
	char key[key_size];
	int key_fd;
	int i;
	char *IV;

	key_fd = open("my.key", O_RDONLY);
	if (key_fd < 0) error("encryption setup");
	i = read(key_fd, key, key_size);
	if (i != key_size) error("encryption setup");

	td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
	if (td == MCRYPT_FAILED) error("encryption setup");


	IV = malloc(mcrypt_enc_get_iv_size(td));
	srand(SEED);
	for (i = 0; i < mcrypt_enc_get_iv_size(td); i++) {
    	IV[i] = rand();
  	}

	i = mcrypt_generic_init(td, key, key_size, IV);
	if (i < 0) error("encryption setup");
}

void encrypt(char *data, int len){
	if (mcrypt_generic(td, data, len) < 0){
		error("encrypt");
	}
}

void decrypt(char *data, int len){
	if (mdecrypt_generic(td, data, len) < 0){
		error("decrypt");
	}
}

struct read_shell_args {
	int shell_to_server_pipe;
	int sock_fd;
};

void *read_shell_output(void *args){
	struct read_shell_args *s_args = (struct read_shell_args *)args;

	int n_bytes;
	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);

	//Server reads from shell process and writes it to the socket
	while ((n_bytes = read(s_args->shell_to_server_pipe, buf, BUF_SIZE)) > 0){
		if (encrypt_flag == 1){
			encrypt(buf, n_bytes);
		}
		write(s_args->sock_fd, buf, n_bytes);
		bzero(buf, BUF_SIZE);
	}

	fprintf(stdout, "Done reading from shell\n");
	if (n_bytes < 0) error("read");

	close(s_args->sock_fd);
	//on EOF exit with code 1
	exit(1);

	return NULL;
}

int main(int argc, char *argv[]){
	parse_options(argc, argv);

	if (encrypt_flag == 1) setup_encryption();

	int sock_fd, newsock_fd;
	socklen_t clilen;

	struct sockaddr_in serv_addr, cli_addr;

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("Error opening socket");

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_no);

	fprintf(stdout, "Binding to socket\n");
	if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		error("Error binding socket");
	}

	fprintf(stdout, "listening on socket\n");
	listen(sock_fd, 1);

	fprintf(stdout, "accepting connections\n");
	newsock_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &clilen);
	fprintf(stdout, "connection on socket %d\n", newsock_fd);


	if (newsock_fd < 1) error("Error accepting connection");

	int server_to_shell_pipe[2];
	int shell_to_server_pipe[2];
	if (pipe(server_to_shell_pipe) < 0 || pipe(shell_to_server_pipe) < 0) error("pipe");

	int c_pid = fork();
	if (c_pid < 0) error("fork");

	//shell (child) process
	if (c_pid == 0){
		fprintf(stdout, "created child process\n");
		close(server_to_shell_pipe[1]);

		close(STDIN_FILENO);
		dup(server_to_shell_pipe[0]);

		close(shell_to_server_pipe[0]);

		close(STDOUT_FILENO);
		dup(shell_to_server_pipe[1]);

		close(STDERR_FILENO);
		dup(shell_to_server_pipe[1]);

		//exec a new shell
		char *shell_args[2];
		shell_args[0] = strdup("/bin/bash");
		shell_args[1] = NULL;
		execvp(shell_args[0], shell_args);
		
	}
	//server (parent) process
	else {
		close(server_to_shell_pipe[0]);
		close(shell_to_server_pipe[1]);

		//Spin up a new thread to read output back from shell
		pthread_t shell_out_thread;
		struct read_shell_args s_args;
		s_args.shell_to_server_pipe = shell_to_server_pipe[0];
		s_args.sock_fd = newsock_fd;
		pthread_create(&shell_out_thread, NULL, read_shell_output, &s_args);


		int n_bytes;
		char buf[BUF_SIZE];
		bzero(buf, BUF_SIZE);

		while ((n_bytes = read(newsock_fd, buf, BUF_SIZE - 1) > 0)){
			if (encrypt_flag == 1){
				decrypt(buf, n_bytes);
			}
			write(server_to_shell_pipe[1], buf, n_bytes);
			bzero(buf, BUF_SIZE);
		}

     	if (n_bytes < 0) error("ERROR reading from socket");
     	fprintf(stdout, "Done reading from socket\n");
    	
		close(newsock_fd);
		close(sock_fd);
		close(server_to_shell_pipe[1]);
		close(shell_to_server_pipe[0]);
		exit(0);
		//read from socket pipe to child process
	}


}