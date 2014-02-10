#include <stdio.h>
#include <sys/time.h>
#include "algo.h"

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
		millis += 1000;
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


