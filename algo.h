#ifndef __ALGO_H
#define __ALGO_H

typedef unsigned int bufindex_t;
typedef unsigned int buflen_t;
typedef void (*P_LISTENER)(bufindex_t);
typedef void (*P_SEARCH_ALGO)(char*, bufindex_t, buflen_t, bufindex_t, P_LISTENER);

extern P_SEARCH_ALGO search;

#ifdef ALGO_BRUTE_FORCE
void brute_force(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener); 
#endif

#ifdef ALGO_KMP
void prepare_kmp();
void free_kmp();
void prepare_prefix(char *pi);
void kmp(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener); 
#endif

#endif
