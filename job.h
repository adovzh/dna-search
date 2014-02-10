#ifndef __JOB_H
#define __JOB_H

typedef struct _job_t {
	char *buf;
	buflen_t len; 
	bufindex_t offset;
} job_t;

extern pthread_mutex_t queue_lock;
extern pthread_cond_t read_cond;

extern pthread_mutex_t jobs_completed_lock;
extern pthread_cond_t jobs_completed_cond;

void submit_job(job_t *buf);
job_t *request_job(); 
void complete_job();
void wait_jobs_completed();

#endif

