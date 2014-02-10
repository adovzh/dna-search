#ifndef __DNAIO_H
#define __DNAIO_H

void print_current_time(struct timeval *tv);
void print_elapsed_time(struct timeval *start_time, struct timeval *end_time);
long calc_elapsed_time(struct timeval *start_time, struct timeval *end_time);
void found_handler(bufindex_t pos);

#endif
