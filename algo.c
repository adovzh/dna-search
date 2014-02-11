#include <string.h>
#include "algo.h"
#include "needle.h"

#ifdef ALGO_BRUTE_FORCE
void brute_force(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener) {
	char *p;
	buflen_t countdown = len;
	
	for (p = buffer + start; countdown; p++, countdown--) 
		if (!memcmp(p, needle, needle_len))
			listener(len - countdown + offset);
}

P_SEARCH_ALGO search = brute_force;
#endif

#ifdef ALGO_KMP
#include <stdlib.h>

static char *prefix;

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
		while (k > 0 && needle[k] != needle[q]) k = pi[k - 1];
		if (needle[k] == needle[q]) k++;
		pi[q] = k;
	}
}

void kmp(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener) {
	char *p = buffer + start;
	buflen_t q, i;

	for (q = 0, i = 0; i < len; i++, p++) {
		while (q > 0 && needle[q] != *p) q = prefix[q - 1];
		if (needle[q] == *p) q++;
		if (q == needle_len) {
			listener(i - needle_len + 1 + offset);
			q = prefix[q - 1];
		}
	}
}

P_SEARCH_ALGO search = kmp;
#endif

