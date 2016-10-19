#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "SortedList.h"

#define KEY_LENGTH 5

pthread_mutex_t mutex;

volatile int lock = 0;

int opt_yield = 0;

void error(char *msg){
	perror(msg);
	exit(1);
}

void no_op(){}

void mutex_lock(){
	pthread_mutex_lock(&mutex);
}

void mutex_unlock(){
	pthread_mutex_unlock(&mutex);
}

void spin_lock(){
	while(__sync_lock_test_and_set(&lock, 1));
}

void spin_unlock(){
	__sync_lock_release(&lock);
}


struct list_args {
	int iterations;
	SortedListElement_t *elems;
	SortedList_t *list;
	void (*lock_fun)();
	void (*unlock_fun)();
};

void *iterative_list_ops(void *args){
	struct list_args *l_args = (struct list_args*)args;

	for (int i = 0; i < l_args->iterations; i++){
		(*(l_args->lock_fun))();
		SortedList_insert(l_args->list, &(l_args->elems[i]));
		(*(l_args->unlock_fun))();
	}

	(*(l_args->lock_fun))();
	SortedList_length(l_args->list);
	(*(l_args->unlock_fun))();

	for (int i = 0; i < l_args->iterations; i++){
		(*(l_args->lock_fun))();
		SortedListElement_t *found = SortedList_lookup(l_args->list, (l_args->elems[i]).key);
		SortedList_delete(found);
		(*(l_args->unlock_fun))();
	}

	free(l_args);
	pthread_exit(NULL);
}

long elapsed_time(struct timespec *start, struct timespec *finish){
	long sec_time = finish->tv_sec - start->tv_sec;
	long nano_time = finish->tv_nsec - start->tv_nsec;
	return (sec_time * 1000000000) + nano_time;
}

struct options_args {
	int *threads_ptr;
	int *iterations_ptr;
	char syncOpt;
	void (**lock_fun_ptr)();
	void (**unlock_fun_ptr)();
};

void parse_options(int argc, char *argv[], struct options_args *o_args){
	int opt_char;

	struct option long_options[] = {
		{"threads", required_argument, 0, 't'},
		{"iterations", required_argument, 0, 'i'},
		{"sync", required_argument, 0, 's'},
		{"yield", required_argument, 0, 'y'}
	};

	while ((opt_char = getopt_long(argc, argv, "t:i:y:s:", long_options, NULL)) != -1){
		switch (opt_char){
			case 't':
				*(o_args->threads_ptr) = atoi(optarg);
				break;
			case 'i':
				*(o_args->iterations_ptr) = atoi(optarg);
				break;
			case 's':
				o_args->syncOpt = *optarg;
				switch (*optarg){
					case 'm':
						pthread_mutex_init(&mutex,NULL);
						*(o_args->lock_fun_ptr) = &mutex_lock;
						*(o_args->unlock_fun_ptr) = &mutex_unlock;
						break;
					case 's':
						*(o_args->lock_fun_ptr) = &spin_lock;
						*(o_args->unlock_fun_ptr) = &spin_unlock;
						break;
					default:
						break;
				}
				break;
			case 'y':
				for (int i = 0; i < strlen(optarg); i++){
					switch(optarg[i]){
						case 'i':
							opt_yield |= INSERT_YIELD;
							break;
						case 'd':
							opt_yield |= DELETE_YIELD;
							break;
						case 'l':
							opt_yield |= LOOKUP_YIELD;
							break;
						default:
							break;
					}
				}
				break;
			default:
				break;
		}
	}
}

int main(int argc, char *argv[]){
	int num_threads = 1;
	int num_iterations = 1;
	void (*lock_fun)() = &no_op;
	void (*unlock_fun)() = &no_op;


	struct options_args o_args;
	o_args.threads_ptr = &num_threads;
	o_args.iterations_ptr = &num_iterations;
	o_args.lock_fun_ptr = &lock_fun;
	o_args.unlock_fun_ptr = &unlock_fun;

	parse_options(argc, argv, &o_args);
	int num_ops = num_threads * num_iterations * 3;
	// fprintf(stdout, "num_threads %d, num_iterations %d, num_ops %d, sync %c, opt_yield %d\n", 
	// 	num_threads, num_iterations, num_ops, o_args.syncOpt, opt_yield);


	//A stupid amount of work for just the test name
	char test_name[50];
	char *test_prefix = "list-";
	char *none = "none";
	int test_idx = 0;
	for (int i = 0; i < strlen(test_prefix); i++){
		test_name[test_idx] = test_prefix[i];
		test_idx++;
	}

	//Handle yied options
	if ((opt_yield & INSERT_YIELD) > 0){
		test_name[test_idx] = 'i';
		test_idx++;
	}
	if ((opt_yield & DELETE_YIELD) > 0){
		test_name[test_idx] = 'd';
		test_idx++;
	}
	if ((opt_yield & LOOKUP_YIELD) > 0){
		test_name[test_idx] = 'l';
		test_idx++;
	}
	if (opt_yield == 0){
		for (int i = 0; i < strlen(none); i++){
			test_name[test_idx] = none[i];
			test_idx++;
		}
	}
	test_name[test_idx] = '-';
	test_idx++;

	//handle sync options
	if (o_args.syncOpt == 'm'){
		test_name[test_idx] = 'm';
		test_idx++;
	}
	else if (o_args.syncOpt == 's'){
		test_name[test_idx] = 's';
		test_idx++;
	}
	else {
		for (int i = 0; i < strlen(none); i++){
			test_name[test_idx] = none[i];
			test_idx++;
		}
	}
	test_name[test_idx] = '\0';

	//initializes an empty list.
	SortedList_t *list = (SortedList_t *) malloc(sizeof(SortedList_t));
	list->key = NULL;
	list->prev = NULL;
	list->next = NULL;


	char *charset = "abcdefghijklmnopqrstuvwxyz";
	int key_length = KEY_LENGTH;
	SortedListElement_t **list_elements = (SortedListElement_t **) malloc(num_threads * sizeof(SortedListElement_t *));
	for (int i = 0; i < num_threads; i++){
		list_elements[i] = (SortedListElement_t *) malloc(num_iterations * sizeof(SortedListElement_t));
		for (int j = 0; j < num_iterations; j++){
			//Generate random key
			char *key = (char *) malloc((key_length + 1) * sizeof(char));
			for (int k = 0; k < key_length; k++){
				key[k] = charset[rand() % strlen(charset)];
			}
			key[key_length] = '\0';

			list_elements[i][j].key = key;
			list_elements[i][j].prev = NULL;
			list_elements[i][j].next = NULL;
		}
	}


	pthread_t threads[num_threads];

	struct timespec start, finish;
	if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) error("clock_gettime");

	struct list_args *l_args;
	for (int i = 0; i < num_threads; i++){
		l_args = (struct list_args *) malloc(sizeof(struct list_args));
		l_args->iterations = num_iterations;
		l_args->elems = list_elements[i];
		l_args->list = list;
		l_args->lock_fun = lock_fun;
		l_args->unlock_fun = unlock_fun;
		if (pthread_create(&threads[i], NULL, iterative_list_ops, l_args) < 0){
			error("pthread_create");
		}
	}

	void *status;
	for (int i = 0; i < num_threads; i++){
		if (pthread_join(threads[i], &status) < 0){
			error("pthread_join");
		}
	}

	if (clock_gettime(CLOCK_MONOTONIC, &finish) < 0) error("clock_gettime");

	int exit_code = 0;
	if (SortedList_length(list) != 0){
		fprintf(stderr, "ERROR, list length is not 0\n");
		exit_code = 1;
	}
	else {
		long tot_run_time = elapsed_time(&start, &finish);
		long avg_run_time = tot_run_time / num_ops;
		fprintf(stdout, "%s,%d,%d,%d,%d,%lu,%lu\n",
			test_name, num_threads, num_iterations, 1, num_ops, tot_run_time, avg_run_time);
	}

	
	for (int i = 0; i < num_threads; i++){
		for (int j = 0; j < num_iterations; j++){
			free((char *)list_elements[i][j].key);
		}
		free(list_elements[i]);
	}
	free(list_elements);
	free(list);

	exit(exit_code);
}