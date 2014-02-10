#ifndef __THREAD_H
#define __THREAD_H

void *handle_job(void *arg); 
void init_threads();
void destroy_threads();

#endif
