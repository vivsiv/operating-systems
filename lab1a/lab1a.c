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

#define BUF_SIZE 1024

static const char EOT = 4;
static const char INTERRUPT = 3;

static struct termios old_term_settings;

struct read_shell_args {
	int shell_to_kb;
};

void *read_shell_output(void *args){
	struct read_shell_args *sargs = (struct read_shell_args*)args;

	char buf[BUF_SIZE];
	int n_bytes;
	//Read from the shell_to_kb_pipe and write to STDOUT
	while ((n_bytes = read(sargs->shell_to_kb, buf, BUF_SIZE)) > 0){
		write(STDOUT_FILENO, buf, n_bytes);
	}

	if (n_bytes == -1){
		perror("read");
		exit(2);
	}

	//on EOF exit with code 1
	exit(1);

	return NULL;
}

//Restore the old terminal settings and print the shell's exit status 
void cleanup(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term_settings) == -1) {
		perror("tcsetattr");
	}

	int status;
	//Grab the exit status of the shell process
	if (waitpid(-1, &status, WUNTRACED) == -1){
		perror("waitpid");
	}
	
	if (WIFEXITED(status)) {
	    fprintf(stderr, "shell process exited with status=%d\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
	    fprintf(stderr, "shell process killed by signal %d\n", WTERMSIG(status));
	} else if (WIFSTOPPED(status)) {
	    fprintf(stderr, "shell process stopped by signal %d\n", WSTOPSIG(status));
	}

}

void sigpipe_handler(int signal){
	perror("SIGPIPE signal recvd");
	exit(1);
}

int main(int argc, char *argv[]){
	//Setup the atexit handler
	atexit(cleanup);

	//Get the current terminal settings
	if (tcgetattr(STDIN_FILENO, &old_term_settings) == -1) {
		perror("tcgetattr");
		exit(1);
	}
	
	//Set the terminal to non-canonical, non-echo mode
	struct termios new_term_settings;
	new_term_settings = old_term_settings;
	new_term_settings.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term_settings) == -1) {
		perror("tcsetattr");
		exit(1);
	}

	//Check for --shell
	int opt_char;
	int shell_flag;
	struct option long_options[] = {
		{"shell", no_argument, &shell_flag, 1},
		{0, 0, 0, 0}
	};
	while ((opt_char = getopt_long(argc, argv, "", long_options, NULL)) != -1){
		switch (opt_char){
			default:
				break;
		}
	}

	//Handle --shell
	// Pipe 1: keyboard process -> kb_to_shell_pipe -> shell process
	int kb_to_shell_pipe[2];
	// Pipe 2: shell process -> shell_to_kb_pipe -> keyboard process
	int shell_to_kb_pipe[2];
	//process_id of child
	int c_pid;
	if (shell_flag == 1){
		if (pipe(kb_to_shell_pipe) == -1 || pipe(shell_to_kb_pipe) == -1){
			perror("pipe");
			exit(1);
		}

		c_pid = fork();
		if (c_pid < 0){
			perror("fork");
			exit(1);
		}
		//If its the child (shell) process
		else if (c_pid == 0){
			//Close the write-end of the kb_to_shell pipe
			close(kb_to_shell_pipe[1]);
			//Redirect the read-end of the kb_to_shell pipe to the shell process' STDIN
			close(STDIN_FILENO);
			dup(kb_to_shell_pipe[0]);

			//Close the read-end of the shell_to_kb pipe
			close(shell_to_kb_pipe[0]);
			//Redirect the shell process' STDOUT & STDERR to the write-end of the shell_to_kb pipe
			close(STDOUT_FILENO);
			dup(shell_to_kb_pipe[1]);
			close(STDERR_FILENO);
			dup(shell_to_kb_pipe[1]);

			//Start the shell process
			char *myargs[2];
			myargs[0] = strdup("/bin/bash");
			myargs[1] = NULL;
			execvp(myargs[0], myargs);
		}
		//If its the parent (keyboard) process
		else {
			//Close the read-end of the kb_to_shell pipe
			close(kb_to_shell_pipe[0]);

			//Close the write-end of the shell_to_kb pipe
			close(shell_to_kb_pipe[1]);

			//Setup a signal handler to handle a SIGPIPE from the shell process
			signal(SIGPIPE, sigpipe_handler);

			//Create a thread to send the shell process' output back to the keyboard process' STDOUT
			pthread_t shell_out_thread;
			struct read_shell_args sargs;
	    	sargs.shell_to_kb = shell_to_kb_pipe[0];
			pthread_create(&shell_out_thread, NULL, read_shell_output, &sargs);
		}
	}

	

	//In the main thread write stdin to back to stdout and to the shell process if necessary
	char buf[BUF_SIZE];
	int n_bytes;
	while ((n_bytes = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
		for (int i = 0; i < n_bytes; i++){
			char c_out = buf[i];
			switch (c_out) {
				//Map <cr> or <lf>
				case '\r':
				case '\n':
					if (shell_flag == 1){
						//If shell is activated map to <lf> and write to write-end of kb_to_shell pipe
						write(kb_to_shell_pipe[1], "\n", 2);
					}
					//into <cr><lf> then write to STDOUT
					write(STDOUT_FILENO, "\r\n", 2);
					break;
				//Handle ^D by closing the pip and sending a SIGHUP to the shell
				case EOT:
					if (shell_flag == 1){
						close(kb_to_shell_pipe[1]);
						kill(c_pid, SIGHUP);
					}
					exit(1);
				//Handle ^C by sending a SIGINT to the shell
				case INTERRUPT:
					if (shell_flag == 1){
						kill(c_pid, SIGINT);
					}
					break;
				//If not a special case 
				default:
					//Write the character to the write-end of the kb_to_shell pipe
					if (shell_flag == 1){
						write(kb_to_shell_pipe[1], buf + i, 1);
					}
					//Write the character out to STDOUT in the kb process
					write(STDOUT_FILENO, buf + i, 1);
					break;
			}
		}
	}
	exit(0);
}