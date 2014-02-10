#include <stdlib.h>
#include <pthread.h>

#include "algo.h"
#include "dnaio.h"
#include "job.h"

#define THREAD_NUM 17

static pthread_t threads[THREAD_NUM];

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


