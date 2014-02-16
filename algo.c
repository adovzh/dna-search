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

#ifdef ALGO_BM
#include <stdlib.h>
#define ALPHA_LEN 128

static int good_suffix[ALPHA_LEN];
static char *needle_last;

void prepare_bm() {
	char *p;
	int count;

	needle_last = needle + needle_len - 1;
	// good_suffix = calloc(128, sizeof(int));
	bzero(good_suffix, ALPHA_LEN * sizeof(int));
	for (p = needle_last + 1, count = 0; p-- != needle; count++)
		good_suffix[*p] = count;
}

void boyer_moore(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener) {
	char *p = buffer + start;
	char *plast, *nlast;
	int countdown = len;
	int prefix_matched_len;
	int bad_count;
	int shift;

	while (countdown > 0) {
		prefix_matched_len = 0;
		bad_count = needle_len;
		plast = p + needle_len - 1;
		nlast = needle_last;

		while (*plast == *nlast) {
			bad_count--;
			prefix_matched_len++;
			plast--;
			nlast--;
			if (prefix_matched_len == needle_len)
				listener(p - buffer + offset);
		}

		if (!good_suffix[*plast])
			shift = bad_count;
		else {
			shift = good_suffix[*plast];
			shift = (shift - prefix_matched_len < 1) ? 1 : shift - prefix_matched_len;
		}

		p += shift;
		countdown -= shift;
	}	
}

P_SEARCH_ALGO search = boyer_moore;
#endif
