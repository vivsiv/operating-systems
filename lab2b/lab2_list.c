#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "SortedList.h"

#define KEY_LENGTH 5

typedef enum sync {NONE, MUTEX, SPIN} sync_option;

typedef struct locks {
	pthread_mutex_t mutex;
	volatile int spin_lock;
} listLocks;

// pthread_mutex_t mutex;

// volatile int lock = 0;

int opt_yield = 0;

void error(char *msg){
	perror(msg);
	exit(1);
}

long elapsed_time(struct timespec *start, struct timespec *finish){
	long sec_time = finish->tv_sec - start->tv_sec;
	long nano_time = finish->tv_nsec - start->tv_nsec;
	return (sec_time * 1000000000) + nano_time;
}

void no_op(){}

void mutex_lock(pthread_mutex_t *mutex){
	pthread_mutex_lock(mutex);
}

void mutex_unlock(pthread_mutex_t *mutex){
	pthread_mutex_unlock(mutex);
}

void spin_lock(volatile int *spin_lock){
	while(__sync_lock_test_and_set(spin_lock, 1));
}

void spin_unlock(volatile int *spin_lock){
	__sync_lock_release(spin_lock);
}


struct list_args {
	int iterations;
	SortedListElement_t *elems;
	SortedList_t *sub_list;
	sync_option sync;
	long lock_time;
	listLocks *locks;
};

void *iterative_list_ops(void *args){
	struct list_args *l_args = (struct list_args*)args;
	struct timespec start, finish;

	for (int i = 0; i < l_args->iterations; i++){
		switch (l_args->sync){
			case NONE:
				SortedList_insert(l_args->sub_list, &(l_args->elems[i]));
				break;
			case MUTEX:
				if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) error("clock_gettime");

				mutex_lock(&(l_args->locks->mutex));

				if (clock_gettime(CLOCK_MONOTONIC, &finish) < 0) error("clock_gettime");
				l_args->lock_time += elapsed_time(&start, &finish);

				SortedList_insert(l_args->sub_list, &(l_args->elems[i]));

				mutex_unlock(&(l_args->locks->mutex));
				break;
			case SPIN:
				spin_lock(&(l_args->locks->spin_lock));
				SortedList_insert(l_args->sub_list, &(l_args->elems[i]));
				spin_unlock(&(l_args->locks->spin_lock));
				break;
			default:
				SortedList_insert(l_args->sub_list, &(l_args->elems[i]));
				break;
		}
	}

	switch (l_args->sync){
		case NONE:
			SortedList_length(l_args->sub_list);
			break;
		case MUTEX:
			if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) error("clock_gettime");

			mutex_lock(&(l_args->locks->mutex));

			if (clock_gettime(CLOCK_MONOTONIC, &finish) < 0) error("clock_gettime");
			l_args->lock_time += elapsed_time(&start, &finish);

			SortedList_length(l_args->sub_list);

			mutex_unlock(&(l_args->locks->mutex));
			break;
		case SPIN:
			spin_lock(&(l_args->locks->spin_lock));
			SortedList_length(l_args->sub_list);
			spin_unlock(&(l_args->locks->spin_lock));
			break;
		default:
			SortedList_length(l_args->sub_list);
			break;
	}

	SortedListElement_t *found;
	for (int i = 0; i < l_args->iterations; i++){
		switch (l_args->sync){
			case NONE:
				found = SortedList_lookup(l_args->sub_list, (l_args->elems[i]).key);
				SortedList_delete(found);
				break;
			case MUTEX:
				if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) error("clock_gettime");

				mutex_lock(&(l_args->locks->mutex));
				if (clock_gettime(CLOCK_MONOTONIC, &finish) < 0) error("clock_gettime");
				l_args->lock_time += elapsed_time(&start, &finish);

				found = SortedList_lookup(l_args->sub_list, (l_args->elems[i]).key);
				SortedList_delete(found);

				mutex_unlock(&(l_args->locks->mutex));
				break;
			case SPIN:
				spin_lock(&(l_args->locks->spin_lock));
				found = SortedList_lookup(l_args->sub_list, (l_args->elems[i]).key);
				SortedList_delete(found);
				spin_unlock(&(l_args->locks->spin_lock));
				break;
			default:
				found = SortedList_lookup(l_args->sub_list, (l_args->elems[i]).key);
				SortedList_delete(found);
				break;
		}
	}

	// free(l_args);
	pthread_exit(NULL);
}

struct options_args {
	int *threads_ptr;
	int *iterations_ptr;
	char syncOpt;
	sync_option *sync_ptr;
	int *num_lists_ptr;
};

void parse_options(int argc, char *argv[], struct options_args *o_args){
	int opt_char;

	struct option long_options[] = {
		{"threads", required_argument, 0, 't'},
		{"iterations", required_argument, 0, 'i'},
		{"sync", required_argument, 0, 's'},
		{"yield", required_argument, 0, 'y'},
		{"lists", required_argument, 0, 'l'}
	};

	while ((opt_char = getopt_long(argc, argv, "t:i:y:s:l:", long_options, NULL)) != -1){
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
						*(o_args->sync_ptr) = MUTEX;
						break;
					case 's':
						*(o_args->sync_ptr) = SPIN;
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
			case 'l':
				*(o_args->num_lists_ptr) = atoi(optarg);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char *argv[]){
	int num_threads = 1;
	int num_iterations = 1;
	sync_option sync = NONE;
	int num_lists = 1;


	struct options_args o_args;
	o_args.threads_ptr = &num_threads;
	o_args.iterations_ptr = &num_iterations;
	o_args.sync_ptr = &sync;
	o_args.num_lists_ptr = &num_lists;

	parse_options(argc, argv, &o_args);
	int num_ops = num_threads * num_iterations * 3;
	// fprintf(stdout, "num_threads %d, num_iterations %d, num_ops %d, sync %c, opt_yield %d\n", 
	// 	num_threads, num_iterations, num_ops, o_args.syncOpt, opt_yield);

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
	SortedList_t *lists = (SortedList_t *) malloc(num_lists * sizeof(SortedList_t));
	listLocks locks_arr[num_lists];
	for (int i = 0; i < num_lists; i++){
		lists[i].key = NULL;
		lists[i].prev = NULL;
		lists[i].next = NULL;
		pthread_mutex_init(&locks_arr[i].mutex, NULL);
		locks_arr[i].spin_lock = 0;
	}



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

	struct list_args *l_args_arr = (struct list_args*) malloc(num_threads * sizeof(struct list_args));
	for (int i = 0; i < num_threads; i++){
		l_args_arr[i].iterations = num_iterations;
		l_args_arr[i].elems = list_elements[i];
		l_args_arr[i].sub_list = &lists[i % num_lists];
		l_args_arr[i].sync = sync;
		l_args_arr[i].lock_time = 0;
		l_args_arr[i].locks = &locks_arr[i % num_lists];
		if (pthread_create(&threads[i], NULL, iterative_list_ops, &l_args_arr[i]) < 0){
			error("pthread_create");
		}
	}

	void *status;
	long total_lock_time = 0;
	for (int i = 0; i < num_threads; i++){
		if (pthread_join(threads[i], &status) < 0){
			error("pthread_join");
		}
		total_lock_time += l_args_arr[i].lock_time;
	}
	free(l_args_arr);

	if (clock_gettime(CLOCK_MONOTONIC, &finish) < 0) error("clock_gettime");

	int error = 0;
	for (int i = 0; i < num_lists; i++){
		if (SortedList_length(&lists[i]) != 0){
			error = 1;
			break;
		}
	}
	
	if (error == 1){
		fprintf(stderr, "ERROR, list length is not 0\n");
	}
	else {
		long tot_run_time = elapsed_time(&start, &finish);
		long avg_run_time = tot_run_time / num_ops;
		long avg_lock_time = total_lock_time / num_ops;
		fprintf(stdout, "%s,%d,%d,%d,%d,%d,%lu,%lu,%lu\n",
			test_name, num_lists, num_threads, num_iterations, 1, num_ops, tot_run_time, avg_run_time, avg_lock_time);
	}
	
	for (int i = 0; i < num_threads; i++){
		for (int j = 0; j < num_iterations; j++){
			free((char *)list_elements[i][j].key);
		}
		free(list_elements[i]);
	}
	free(list_elements);

	for (int i = 0; i < num_lists; i++){
		pthread_mutex_destroy(&(locks_arr[i].mutex));
	}
	free(lists);

	exit(0);
}