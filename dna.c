#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "algo.h"
#include "needle.h"
#include "dnaio.h"
#include "job.h"
#include "thread.h"

#define ERROR_OCCURRED(s) { perror(s); exit(EXIT_FAILURE); }
#define ASSERT_NOT(cond,msg) if ((cond)) { printf("%s\n", (msg)); exit(EXIT_FAILURE); }

int main(int argc, char **argv) {
	init_needle(argc > 1 ? argv[1] : "GCAACGAGTGTCTTTG");	
#ifdef DNA_DEBUG
	printf("Looking for %s\n", needle);
#endif

	struct timeval start_time;
	print_current_time(&start_time);
	printf(" START\n");

	bufindex_t input_size = 3000000000L;
	size_t page_size = sysconf(_SC_PAGESIZE);
	ASSERT_NOT(page_size & (page_size - 1), "Page is not a power of 2");

	size_t mask = page_size - 1;
	size_t mem_size = (input_size | mask) + 1;
	buflen_t buf_size = page_size << 14;

#ifdef DNA_DEBUG
	printf("Buf size = %u\n", buf_size);;
#endif

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

	int fd = 0;
	if (argc > 2 && (fd = open(argv[2], O_RDONLY)) == -1) ERROR_OCCURRED("open");
	char *mapped_file = mmap(NULL, mem_size, PROT_READ, MAP_SHARED, fd, 0);
	if (fd > 0 && close(fd) == -1) ERROR_OCCURRED("close");
	char *buf;

	bufindex_t start;
	buflen_t bytes_read;
	buflen_t len = buf_size + needle_len - 1;

	for (start = 0, buf = mapped_file; start < input_size; start += buf_size, buf += buf_size) {
		bytes_read = (start + len < input_size) ? len : input_size - start;
		job_t *job = malloc(sizeof(job_t));
		job->buf = buf;
		job->len = bytes_read;
		job->offset = start;
		submit_job(job);
	}

#ifdef DNA_DEBUG	
	print_current_time(&ty);
	printf(" End submitting jobs\n");
	io_time = calc_elapsed_time(&tx, &ty);
#endif

	wait_jobs_completed();
	munmap(mapped_file, mem_size);

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

	return 0;
}
