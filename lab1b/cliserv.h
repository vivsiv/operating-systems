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

#define SEED 14

void error(char *msg){
	perror(msg);
	exit(1);
}

void encrypt(MCRYPT td, char *data, int len){
	if (mcrypt_generic(td, data, len) < 0){
		error("encrypt");
	}
}

void decrypt(MCRYPT td, char *data, int len){
	if (mdecrypt_generic(td, data, len) < 0){
		error("decrypt");
	}
}
