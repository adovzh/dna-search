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
#include <stdio.h>
#define ALPHA_LEN 128

static int bad_char[ALPHA_LEN];
static int *good_suffix;
static char *needle_last;

void prepare_bm() {
	needle_last = needle + needle_len - 1;
	prepare_bad_char();
	prepare_good_suffix();
}

void prepare_bad_char() {
	int i;
	for (i = 0; i < ALPHA_LEN; i++) bad_char[i] = needle_len;
	for (i = 0; i < needle_len - 1; i++) bad_char[(int)needle[i]] = needle_len - 1 - i;
}

#define max(a, b) ((a < b) ? (b) : (a))

int is_prefix(char *word, int wordlen, int pos) {
	char *s = word + pos;
	char *limit = word + (wordlen - pos);
	while (word != limit)
		if (*word++ != *s++)
			return 0;
	return 1;
}

int suffix_length(char *word, int wordlen, int pos) {
	int i;
	char *p = word + pos;
	char *q = word + wordlen;

	for (i = 0; *p-- == *--q && i < pos; i++);
	return i;
	/*
	int i;
	for (i = 0; word[pos - i] == word[wordlen - 1 - i] && i < pos; i++);
	return i;
	*/
}

void prepare_good_suffix() {
	int p;
	int last_prefix_index = needle_len - 1;

	good_suffix = malloc(needle_len * sizeof(int));

	for (p = needle_len - 1; p >=0; p--) {
		if (is_prefix(needle, needle_len, p + 1))
			last_prefix_index = p + 1;
		good_suffix[p] = last_prefix_index + (needle_len - 1 - p);
	}

	for (p = 0; p < needle_len - 1; p++) {
		int slen = suffix_length(needle, needle_len, p);
		if (needle[p - slen] != needle[needle_len - 1 - slen])
			good_suffix[needle_len - 1 - slen] = needle_len - 1 - p + slen;
	}
}

void free_bm() {
	free(good_suffix);
}

void boyer_moore(char *buffer, bufindex_t start, buflen_t len, bufindex_t offset, P_LISTENER listener) {
	int i = needle_len - 1;
	char *p = buffer + start;

	while (i < len) {
		int j = needle_len - 1;
		while (j >= 0 && p[i] == needle[j]) {
			--i;
			--j;
		}

		if (j < 0) {
			listener(i + 1 + offset);
			i+=good_suffix[0] + 1;
		} else {
			i += max(bad_char[(int)p[i]], good_suffix[j]); }
	}
}

P_SEARCH_ALGO search = boyer_moore;
#endif
