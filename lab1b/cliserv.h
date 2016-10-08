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

static MCRYPT td;

#define SEED 14

void error(char *msg){
	perror(msg);
	exit(1);
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
