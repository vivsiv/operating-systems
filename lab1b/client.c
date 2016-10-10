#include "cliserv.h"

static struct termios old_term_settings;
static int port_no;
static char *log_file;
static int encrypt_flag = 0;
static MCRYPT td;
static char *IV = NULL;

pthread_mutex_t log_mutex;

#define BUF_SIZE 256
#define HOST_NAME "localhost"

//Set the terminal to non-canonical, non-echo mode
void setup_terminal(){
	if (tcgetattr(STDIN_FILENO, &old_term_settings) == -1) {
		error("tcgetattr");
	}
	
	struct termios new_term_settings;
	new_term_settings = old_term_settings;
	new_term_settings.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term_settings) == -1) {
		error("tcsetattr");
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
				break;
			case 'l':
				log_file = optarg;
				fprintf(stdout, "Got log file: %s from options\n", log_file);
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

struct read_socket_args {
	int sock_fd;
	int log_fd;
};

void log_to_file(char *type, int log_fd, char *log_msg){
	if (strlen(log_msg) > 0){
		char bytes_str[BUF_SIZE];
		bzero(bytes_str, BUF_SIZE);
		sprintf(bytes_str, "%s %lu bytes: ", type, strlen(log_msg));
		
		write(log_fd, bytes_str, strlen(bytes_str));
		write(log_fd, log_msg, strlen(log_msg));
		write(log_fd, "\n", 1);
	}
}

void *read_socket(void *args){
	struct read_socket_args *s_args = (struct read_socket_args*)args;

	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);
	int n_bytes;

	char log_msg[BUF_SIZE];
	bzero(log_msg, BUF_SIZE);
	int log_idx = 0;
	while ((n_bytes = read(s_args->sock_fd, buf, BUF_SIZE - 1)) > 0){
		if (encrypt_flag == 1){
			decrypt(td, buf, n_bytes);
		}
		for (int i = 0; i < n_bytes; i++){
			char c_out = buf[i];
			switch (c_out) {
				//Map <cr> or <lf>
				case '\r':
				case '\n':
					write(STDOUT_FILENO, "\r\n", 2);
					if (s_args->log_fd >= 0){
						log_msg[log_idx] = '\0';
						pthread_mutex_lock(&log_mutex);
						log_to_file("RECEIVED", s_args->log_fd, log_msg);
						pthread_mutex_unlock(&log_mutex);
						bzero(log_msg, BUF_SIZE);
						log_idx = 0;
					}
					break;
				//Handle ^D
				case '\004':
					close(s_args->sock_fd);
					if (s_args->log_fd >= 0) {
						log_msg[log_idx] = '\0';
						pthread_mutex_lock(&log_mutex);
						log_to_file("RECEIVED", s_args->log_fd, log_msg);
						pthread_mutex_unlock(&log_mutex);
						close(s_args->log_fd);
					}
					exit(0);
				//If not a special case 
				default:
					//Write the character out to STDOUT in the kb process
					write(STDOUT_FILENO, buf + i, 1);
					if (s_args->log_fd >= 0){
						log_msg[log_idx] = c_out;
						log_idx++;
					}
					break;
			}
		}
		bzero(buf, BUF_SIZE);
	}

	//EOF or read error received
	close(s_args->sock_fd);
	exit(1);

	return NULL;
}


int main(int argc, char *argv[]){
	atexit(cleanup_terminal);

	setup_terminal();

	parse_options(argc, argv);

	if (encrypt_flag == 1) {
		setup_encryption(td);
		atexit(end_encryption);
	}

	int sock_fd;
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

	fprintf(stdout, "Connecting to server..\n");
	if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		error("Error connecting to host");
	}
	fprintf(stdout, "Connected to server, start typing commands\n");

	int log_fd = -1;
	if (log_file != NULL) {
		log_fd = open(log_file, O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IXGRP);
		if (log_fd < 0) fprintf(stdout, "Error opening log file: %s\n", log_file);
	}

	//spin up a second thread to read from the socket and write responses back to STDOUT
	pthread_t read_socket_thread;
	struct read_socket_args s_args;
	s_args.sock_fd = sock_fd;
	s_args.log_fd = log_fd;
	pthread_create(&read_socket_thread, NULL, read_socket, &s_args);

	//in the main thread we read from the keyboard and write to the socket
	char buf[BUF_SIZE];
	int n_bytes;
	bzero(buf, BUF_SIZE);

	char log_msg[BUF_SIZE];
	bzero(log_msg, BUF_SIZE);
	int log_idx = 0;
	while ((n_bytes = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
		for (int i = 0; i < n_bytes; i++){
			char c_out = buf[i];
			switch (c_out) {
				//Map <cr> or <lf>
				case '\r':
				case '\n':
					;
					char new_line = '\n';
					if (encrypt_flag == 1) encrypt(td, &new_line, 1);
					write(sock_fd, &new_line, 1);

					if (log_fd >= 0){
						log_msg[log_idx] = '\0';
						pthread_mutex_lock(&log_mutex);
						log_to_file("SENT", log_fd, log_msg);
						pthread_mutex_unlock(&log_mutex);
						bzero(log_msg, BUF_SIZE);
						log_idx = 0;
					}
					write(STDOUT_FILENO, "\r\n", 2);
					break;
				//Handle ^D
				case '\004':
					pthread_cancel(read_socket_thread);
					fprintf(stdout, "EOF received, closing socket\n");
					close(sock_fd);
					if (log_fd >= 0) {
						log_msg[log_idx] = '\0';
						log_to_file("SENT", log_fd, log_msg);
						close(log_fd);
					}
					exit(0);
				default:
					//Write the character out to STDOUT in the kb process
					write(STDOUT_FILENO, buf + i, 1);

					if (log_fd >= 0){
						log_msg[log_idx] = c_out;
						log_idx++;
					}

					if (encrypt_flag == 1){
						encrypt(td, buf + i , 1);
					}
					
					write(sock_fd, buf + i, 1);
					
					break;
			}
		}
		bzero(buf, BUF_SIZE);
	}
	exit(0);

}