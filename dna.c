#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

typedef unsigned int bufindex_t;
typedef int buflen_t;
typedef void (*P_LISTENER)(bufindex_t);
typedef void (*P_SEARCH_ALGO)(char*, bufindex_t, buflen_t, bufindex_t, P_LISTENER);

char *needle = "GCAACGAGTGTCTTTG";
int needle_len;

void brute_force(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener) {
	char *p;
	buflen_t countdown = len;

	for (p = buffer + start; countdown; p++, countdown--) 
		if (!memcmp(p, needle, needle_len))
			listener(len - countdown + offset);
}

char *prefix;
void prepare_prefix(char*);

void prepare_kmp() {
	prefix = malloc(needle_len);
	prepare_prefix(prefix);
}

void free_kmp() {
	free(prefix);
}

void prepare_prefix(char *pi) {
	int k, q;

	pi[0] = 0;

	for (k = 0, q = 1; q < needle_len; q++) {
		while (k > 0 && needle[k] != needle[q]) k = pi[q];
		if (needle[k] == needle[q]) k++;
		pi[q] = k;
		printf("pi[%d] = %d\n", q, k);
	}
}

void kmp(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener) {
	char *cur = buffer + start;
	buflen_t q, i;

	for (q = 0, i = 0; i < len; i++, cur++) {
		while (q > 0 && needle[q] != *cur) q = prefix[q];
		if (needle[q] == *cur) q++;
		if (q == needle_len) {
			printf("kmp found: ");
			listener(i - needle_len + 1 + offset);
			q = prefix[q];
		}
	}
}

void print_current_time(struct timeval *tv) {
	struct tm time_tm;
	char formatted[78];

	gettimeofday(tv, NULL);
	localtime_r(&tv->tv_sec, &time_tm);
	strftime(formatted, sizeof formatted, "%b%e %H:%M:%S", &time_tm);
	printf("%s.%03d", formatted, (int)(tv->tv_usec / 1000));
}

void print_elapsed_time(struct timeval *start_time, struct timeval *end_time) {
	int seconds = end_time->tv_sec - start_time->tv_sec;
	int millis = (end_time->tv_usec - start_time->tv_usec) / 1000;
	if (millis < 0) {
		seconds--;
		millis = 1000 - millis;
	}
	printf("Elapsed: %d.%03d\n", seconds, millis);
}

long calc_elapsed_time(struct timeval *start_time, struct timeval *end_time) {
	return (end_time->tv_sec - start_time->tv_sec) * 1000000 + end_time->tv_usec - start_time->tv_usec;
}

void found_handler(bufindex_t pos) {
	struct timeval tv;
	print_current_time(&tv);
	printf(" FOUND SEQUENCE STARTING AT %u\n", pos);
}

#define MAX_JOB_QUEUE 6000

typedef struct _job_t {
	char *buf;
	buflen_t len; 
	bufindex_t offset;
} job_t;

job_t *job_queue[MAX_JOB_QUEUE];
int head = 0, tail = 0;
int jobs_posted = 0;
int jobs_completed = 0;
pthread_mutex_t queue_lock;
pthread_cond_t read_cond;

pthread_mutex_t jobs_completed_lock;
pthread_cond_t jobs_completed_cond;

void submit_job(job_t *buf) {
	pthread_mutex_lock(&queue_lock);
	job_queue[tail++] = buf;
	pthread_cond_broadcast(&read_cond);
	pthread_mutex_unlock(&queue_lock);
	jobs_posted++;
}

job_t *request_job() {
	pthread_mutex_lock(&queue_lock);
	while (head == tail)
		pthread_cond_wait(&read_cond, &queue_lock);
	job_t *job = job_queue[head++];
	pthread_mutex_unlock(&queue_lock);
	
	return job;
}

void complete_job() {
	pthread_mutex_lock(&jobs_completed_lock);
	jobs_completed++;
	pthread_cond_broadcast(&jobs_completed_cond);
	pthread_mutex_unlock(&jobs_completed_lock);
}

void wait_jobs_completed() {
	int errorno = 0;
	struct timeval tv;
	struct timespec timeout;
	gettimeofday(&tv, NULL);
	timeout.tv_sec = tv.tv_sec + 90;
   	timeout.tv_nsec = 0;

	pthread_mutex_lock(&jobs_completed_lock);
	while (!errorno && jobs_posted > jobs_completed)
		errorno = pthread_cond_timedwait(&jobs_completed_cond, &jobs_completed_lock, &timeout);
	if (errorno){
		printf("Timed wait: %d, %s\n", errorno, strerror(errorno));
	}
	pthread_mutex_unlock(&jobs_completed_lock);
}

P_SEARCH_ALGO search = brute_force;
// P_SEARCH_ALGO search = kmp;

#define THREAD_NUM 17 

pthread_t threads[THREAD_NUM];

void *handle_job(void *arg) {
	while (1) {
		job_t *job = request_job();
		search(job->buf, 0, job->len, job->offset, found_handler);
		free(job->buf);
		free(job);
		complete_job();
	}
}

void init_threads() {
	int i;
	pthread_t *p = threads;

	pthread_mutex_init(&queue_lock, NULL);
	pthread_cond_init(&read_cond, NULL);
	pthread_mutex_init(&jobs_completed_lock, NULL);
	pthread_cond_init(&jobs_completed_cond, NULL);

	for (i = 0; i < THREAD_NUM; i++, p++)
		pthread_create(p, NULL, handle_job, NULL);
}

void destroy_threads() {
	int i;
	pthread_t *p = threads;

	for (i = 0; i < THREAD_NUM; i++, p++)
		pthread_cancel(*p);

	pthread_cond_destroy(&jobs_completed_cond);
	pthread_mutex_destroy(&jobs_completed_lock);
	pthread_cond_destroy(&read_cond);
	pthread_mutex_destroy(&queue_lock);
}

int main() {
	struct timeval start_time;
	print_current_time(&start_time);
	printf(" START\n");

	bufindex_t input_size = 3000000000L;
	buflen_t buf_size = 500000;
	// long buf_size = input_size;
	needle_len = strlen(needle);

	init_threads();

	// prepare_kmp();

	// long mem_time = 0, working_time = 0;
	long io_time, waiting_time;
	struct timeval tx, ty, tz;
	// gettimeofday(&tx, NULL);
	bufindex_t start;
	
	print_current_time(&tx);
	printf(" Start reading data\n");

	for (start = 0; start < input_size; start += buf_size) {
		// printf("Allocating %d starting from %u\n", buf_size + needle_len, start);
		// struct timeval st, et, at, at2;
		// gettimeofday(&st, NULL);

		char *buf = malloc(buf_size + needle_len);
		buflen_t bytes_read = fread(buf, 1, buf_size + needle_len, stdin);
		// printf("%ld bytes read\n", bytes_read);

		// gettimeofday(&et, NULL);
		// mem_time += calc_elapsed_time(&st, &et);

		if (bytes_read) {
			// search(buf, 0, bytes_read, start, found_handler);
			job_t *job = malloc(sizeof(job_t));
			job->buf = buf;
			job->len = bytes_read;
			job->offset = start;
			submit_job(job);
		}
		else
			printf("ERROR\n");

		// gettimeofday(&at, NULL);
		// working_time += calc_elapsed_time(&et, &at);
		
		buflen_t rollback = buf_size - bytes_read;
		if (rollback < 0)
			fseek(stdin, -needle_len, SEEK_CUR);
		// free(buf);
		// gettimeofday(&at2, NULL);
		// mem_time += calc_elapsed_time(&at, &at2);
	}

	// gettimeofday(&ty, NULL);
	print_current_time(&ty);
	printf(" End submitting jobs\n");
	io_time = calc_elapsed_time(&tx, &ty);

	wait_jobs_completed();

	gettimeofday(&tz, NULL);
	waiting_time = calc_elapsed_time(&ty, &tz);

	destroy_threads();

	// free_kmp();

	struct timeval end_time;
	print_current_time(&end_time);
	printf(" FINISH\n");

	print_elapsed_time(&start_time, &end_time);
	printf("I/O: %ld\n", io_time);
	printf("Waiting: %ld\n", waiting_time);
}
