#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include "algo.h"
#include "needle.h"
#include "dnaio.h"
#include "job.h"
#include "thread.h"

int main(int argc, char **argv) {
	init_needle(argc > 1 ? argv[1] : "GCAACGAGTGTCTTTG");	
#ifdef DNA_DEBUG
	printf("Looking for %s\n", needle);
#endif

	struct timeval start_time;
	print_current_time(&start_time);
	printf(" START\n");

	bufindex_t input_size = 3000000000L;
	buflen_t buf_size = 500000;
		
	init_threads();

#ifdef ALGO_KMP	
	prepare_kmp();
#endif

#ifdef DNA_DEBUG
	long io_time, waiting_time;
	struct timeval tx, ty, tz;	
	print_current_time(&tx);
	printf(" Start reading data\n");
#endif

	bufindex_t start;
	for (start = 0; start < input_size; start += buf_size) {
		char *buf = malloc(buf_size + needle_len);
		buflen_t bytes_read = fread(buf, 1, buf_size + needle_len - 1, stdin);

		if (bytes_read) {
			job_t *job = malloc(sizeof(job_t));
			job->buf = buf;
			job->len = bytes_read;
			job->offset = start;
			submit_job(job);
		}
		else
			printf("ERROR\n");

		buflen_t rollback = buf_size - bytes_read;

		if (rollback < 0)
			fseek(stdin, rollback, SEEK_CUR);
	}

#ifdef DNA_DEBUG	
	print_current_time(&ty);
	printf(" End submitting jobs\n");
	io_time = calc_elapsed_time(&tx, &ty);
#endif

	wait_jobs_completed();

#ifdef DNA_DEBUG
	gettimeofday(&tz, NULL);
	waiting_time = calc_elapsed_time(&ty, &tz);
#endif

	destroy_threads();

#ifdef ALGO_KMP	
	free_kmp();
#endif

	struct timeval end_time;
	print_current_time(&end_time);
	printf(" FINISH\n");

#ifdef DNA_DEBUG
	print_elapsed_time(&start_time, &end_time);
	printf("I/O: %ld\n", io_time);
	printf("Waiting: %ld\n", waiting_time);
#endif
}
