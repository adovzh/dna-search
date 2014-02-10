#include <string.h>
#include "needle.h"

char *needle; 
int needle_len;

void init_needle(char *s) {
	needle = s;
	needle_len = strlen(s);
}


