#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 3000000000L
#define DEFAULT_SEED 1234

char alphabet[] = "ACTG";

int main(int argc, char **argv) {
	int alpha_len = strlen(alphabet);
	long bound = INPUT_SIZE;
	unsigned seed = argc > 1 ? atoi(argv[1]) : DEFAULT_SEED;
	long i;

	srandom(seed);

	for (i = 0; i < bound; i++) 
		putchar(alphabet[random() % alpha_len]);

	return 0;
}
