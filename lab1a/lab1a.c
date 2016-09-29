#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define STDIN 0
#define STDOUT 1
#define BUF_SIZE 1024

static const char EOT = 4;

int main(int argc, char *argv[]){
	struct termios old_term_settings;
	struct termios new_term_settings;

	int err;
	//Get the current terminal settings
	err = tcgetattr(STDIN, &old_term_settings);
	if (err > 0) {
		perror("Error getting old stdin terminal settings");
		exit(1);
	}

	new_term_settings = old_term_settings;
	//set the terminal to non-canonical, non-echo mode

	new_term_settings.c_lflag &= ~(ICANON | ECHO);
	err = tcsetattr(STDIN, TCSAFLUSH, &new_term_settings);
	if (err > 0) {
		perror("Error setting new stdin terminal settings");
		exit(1);
	}

	char buf[BUF_SIZE];
	int n_bytes;
	while ((n_bytes = read(STDIN, buf, BUF_SIZE)) > 0){
		for (int i = 0; i < n_bytes; i++){
			char c_out = buf[i];
			switch (c_out) {
				//Map <cr> or <lf> into <cr><lf> then write to stdout
				case '\r':
				case '\n':
					write(STDOUT, "\r\n", 2);
					break;
				//Handle ^D by restoring old terminal attributes and exiting
				case EOT:
					err = tcsetattr(STDIN, TCSAFLUSH, &old_term_settings);
					if (err > 0) {
						perror("Error restoring old stdin terminal settings");
						exit(1);
					}
					else {
						exit(0);
					}
				//If not a special case just write the character out to stdout
				default:
					write(STDOUT, buf + i, 1);
					break;
			}
		}
	}
    exit(0);
}