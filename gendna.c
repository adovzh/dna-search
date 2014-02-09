#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 3000000000L

char alphabet[] = "ACTG";

int main() {
	int alpha_len = strlen(alphabet);
	long bound = INPUT_SIZE;

	for (long i = 0; i < bound; i++) 
		putchar(alphabet[random() % alpha_len]);

	return 0;
}
