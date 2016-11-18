#include "cliserv.h"

#define BUF_SIZE 256

static int port_no = 0;
static int encrypt_flag = 0;
static int c_pid = -1;
static MCRYPT td;
static char *IV = NULL;

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

void end_encryption(){
	mcrypt_generic_end(td);
	if (IV != NULL) free(IV);
}

void cleanup_shell(){
	kill(c_pid, SIGHUP);
}

struct read_shell_args {
	int shell_to_server_pipe;
};

void *read_shell_output(void *args){
	struct read_shell_args *s_args = (struct read_shell_args *)args;

	int n_bytes;
	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);

	//Server reads from shell process and writes it to the socket
	while ((n_bytes = read(s_args->shell_to_server_pipe, buf, BUF_SIZE)) > 0){
		if (encrypt_flag == 1){
			encrypt(td, buf, n_bytes);
		}
		write(STDOUT_FILENO, buf, n_bytes);
		bzero(buf, BUF_SIZE);
	}

	//EOF received from shell
	if (n_bytes == 0) {
		exit(2);
	}
	//Read error
	else {
		exit(1);
	}

	return NULL;
}

void sigpipe_handler(int signal){
	exit(2);
}

int main(int argc, char *argv[]){
	parse_options(argc, argv);

	if (encrypt_flag == 1) {
		setup_encryption();
		atexit(end_encryption);
	}

	//One socket for listening, the other for accepting a connection
	int sock_fd, newsock_fd;

	struct sockaddr_in serv_addr;

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("Error opening socket");

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_no);

	if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		error("Error binding socket");
	}

	listen(sock_fd, 1);
	fprintf(stdout, "Listening on port %d for connections\n", port_no);

	newsock_fd = accept(sock_fd, NULL, NULL);
	fprintf(stdout, "Received Connection\n");
	//Since we're only accepting one connection, we can just close the listening socket
	close(sock_fd);
	// printf("%d\n",newsock_fd);
	if (newsock_fd < 0) error("Error accepting connection");
	
	int server_to_shell_pipe[2];
	int shell_to_server_pipe[2];
	if (pipe(server_to_shell_pipe) < 0 || pipe(shell_to_server_pipe) < 0) {
		close(newsock_fd);
		error("pipe");
	}

	c_pid = fork();
	if (c_pid < 0) {
		close(newsock_fd);
		error("fork");
	}

	atexit(cleanup_shell);

	//shell (child) process
	if (c_pid == 0){
		close(server_to_shell_pipe[1]);
		close(shell_to_server_pipe[0]);

		close(STDIN_FILENO);
		dup(server_to_shell_pipe[0]);
		close(server_to_shell_pipe[0]);

		close(STDOUT_FILENO);
		dup(shell_to_server_pipe[1]);
		close(STDERR_FILENO);
		dup(shell_to_server_pipe[1]);

		close(shell_to_server_pipe[1]);

		//exec a new shell
		char *shell_args[2];
		shell_args[0] = strdup("/bin/bash");
		shell_args[1] = NULL;
		if (execvp(shell_args[0], shell_args) < 0) error("execvp shell");
		
	}
	//server (parent) process
	else {
		close(server_to_shell_pipe[0]);
		close(shell_to_server_pipe[1]);

		//Redirect server's input/output to the socket
		close(STDIN_FILENO);
		dup(newsock_fd);

		close(STDOUT_FILENO);
		dup(newsock_fd);

		close(STDERR_FILENO);
		dup(newsock_fd);

		close(newsock_fd);

		signal(SIGPIPE, sigpipe_handler);

		//Spin up a new thread to read output back from shell
		pthread_t shell_out_thread;
		struct read_shell_args s_args;
		s_args.shell_to_server_pipe = shell_to_server_pipe[0];
		if (pthread_create(&shell_out_thread, NULL, read_shell_output, &s_args) < 0) {
			error("pthread_create");
		}

		int n_bytes;
		char buf[BUF_SIZE];
		bzero(buf, BUF_SIZE);

		while ((n_bytes = read(STDIN_FILENO, buf, BUF_SIZE - 1) > 0)){
			if (encrypt_flag == 1){
				decrypt(td, buf, n_bytes);
			}
			write(server_to_shell_pipe[1], buf, n_bytes);
			bzero(buf, BUF_SIZE);
		}
     	 
		close(server_to_shell_pipe[1]);
		close(shell_to_server_pipe[0]);    	

    	fprintf(stdout, "EOF received: closing connection\n");
		exit(1);
	}

	exit(0);
}