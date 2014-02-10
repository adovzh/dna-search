#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include "algo.h"
#include "job.h"

#define MAX_JOB_QUEUE 6000

static job_t *job_queue[MAX_JOB_QUEUE];
static int head = 0, tail = 0;
static int jobs_posted = 0;
static int jobs_completed = 0;

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


