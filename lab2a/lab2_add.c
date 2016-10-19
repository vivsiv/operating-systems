#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>

pthread_mutex_t mutex;

volatile int lock = 0;

void error(char *msg){
	perror(msg);
	exit(1);
}

void add(long long *pointer, long long value, int opt_yield) {
    long long sum = *pointer + value;
    if (opt_yield) sched_yield();
    *pointer = sum;
}

void add_mutex(long long *pointer, long long value, int opt_yield){
	pthread_mutex_lock(&mutex);
	add(pointer, value, opt_yield);
	pthread_mutex_unlock(&mutex);
}

void add_spinlock(long long *pointer, long long value, int opt_yield){
	while(__sync_lock_test_and_set(&lock,1));
	add(pointer, value, opt_yield);
    __sync_lock_release(&lock);
}

void add_compare_and_swap(long long *pointer, long long value, int opt_yield){
	long long pointer_val;
	long long new_val;
	while(1){
		pointer_val = *pointer;
		new_val = pointer_val + value;
		if (opt_yield) sched_yield();
		if (__sync_val_compare_and_swap(pointer, pointer_val, new_val) == pointer_val) break;
	}
}

struct add_args {
	long long *pointer;
	int iterations;
	int opt_yield;
	void (*add_fun)(long long*, long long, int);
};

void *iterative_add(void *args){
	struct add_args *a_args = (struct add_args*)args;

	for (int i = 0; i < a_args->iterations; i++){
		(*(a_args->add_fun))(a_args->pointer, 1, a_args->opt_yield);
	}

	for (int i = 0; i < a_args->iterations; i++){
		(*(a_args->add_fun))(a_args->pointer, -1, a_args->opt_yield);
	}

	free(a_args);
	pthread_exit(NULL);
}

struct options_args {
	int *threads_ptr;
	int *iterations_ptr;
	int *opt_yield_ptr;
	void (**add_fun_ptr)(long long*, long long, int);
	char syncOpt;
};

void parse_options(int argc, char *argv[], struct options_args *o_args){
	int opt_char;

	struct option long_options[] = {
		{"threads", required_argument, 0, 't'},
		{"iterations", required_argument, 0, 'i'},
		{"yield", no_argument, o_args->opt_yield_ptr, 1},
		{"sync", required_argument, 0, 's'}
	};

	while ((opt_char = getopt_long(argc, argv, "t:i:s:", long_options, NULL)) != -1){
		switch (opt_char){
			case 't':
				*(o_args->threads_ptr) = atoi(optarg);
				break;
			case 'i':
				*(o_args->iterations_ptr) = atoi(optarg);
				break;
			case 's':
				switch(*optarg){
					case 'm':
						pthread_mutex_init(&mutex,NULL);
						*(o_args->add_fun_ptr) = &add_mutex;
						o_args->syncOpt = 'm';
						break;
					case 's':
						*(o_args->add_fun_ptr) = &add_spinlock;
						o_args->syncOpt = 's';
						break;
					case 'c':
						*(o_args->add_fun_ptr) = &add_compare_and_swap;
						o_args->syncOpt = 'c';
						break;
					default:
						break;
				}
			default:
				break;
		}
	}
}



long elapsed_time(struct timespec *start, struct timespec *finish){
	long sec_time = finish->tv_sec - start->tv_sec;
	long nano_time = finish->tv_nsec - start->tv_nsec;
	return (sec_time * 1000000000) + nano_time;
}


int main(int argc, char *argv[]){
	int num_threads = 1;
	int num_iterations = 1;
	int opt_yield = 0;
	void (*add_fun)(long long*, long long, int) = &add;

	struct options_args o_args;
	o_args.threads_ptr = &num_threads;
	o_args.iterations_ptr = &num_iterations;
	o_args.opt_yield_ptr = &opt_yield;
	o_args.add_fun_ptr = &add_fun;

	parse_options(argc, argv, &o_args);

	char *test_name = "add-none";
	switch (o_args.syncOpt){
		case 'm':
			if (opt_yield == 1) test_name = "add-yield-m";
			else test_name = "add-m";
			break;
		case 's':
			if (opt_yield == 1) test_name = "add-yield-s";
			else test_name = "add-s";
			break;
		case 'c':
			if (opt_yield == 1) test_name = "add-yield-c";
			else test_name = "add-c";
			break;
		default:
			if (opt_yield == 1) test_name = "add-yield-none";
			break;
	}


	int num_ops = num_threads * num_iterations * 2;

	long long count = 0;

	pthread_t threads[num_threads];

	struct timespec start, finish;
	if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) error("clock_gettime");

	struct add_args *a_args;
	for (int i = 0; i < num_threads; i++){
		a_args = (struct add_args *) malloc(sizeof(struct add_args));
		a_args->pointer = &count;
		a_args->iterations = num_iterations;
		a_args->opt_yield = opt_yield;
		a_args->add_fun = add_fun;
		if (pthread_create(&threads[i], NULL, iterative_add, a_args) < 0){
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

	long tot_run_time = elapsed_time(&start, &finish);
	long avg_run_time = tot_run_time / num_ops;
	fprintf(stdout, "%s,%d,%d,%d,%lu,%lu,%llu\n", 
		test_name, num_threads, num_iterations, num_ops, tot_run_time, avg_run_time, count);

	pthread_exit(NULL);
}