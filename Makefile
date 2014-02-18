CC=gcc
ALGO ?= BRUTE_FORCE
override CFLAGS += -c -Ofast -Wall -DALGO_$(ALGO)
LDFLAGS=-lpthread -pthread

all: dna gendna

dna: dna.o algo.o needle.o dnaio.o job.o thread.o
	$(CC) $(LDFLAGS) dna.o algo.o needle.o dnaio.o job.o thread.o -o dna

dna.o: dna.c algo.h needle.h dnaio.h job.h
	$(CC) $(CFLAGS) dna.c

algo.o: algo.c algo.h needle.h
	$(CC) $(CFLAGS) algo.c

needle.o: needle.c needle.h
	$(CC) $(CFLAGS) needle.c

dnaio.o: dnaio.c algo.h
	$(CC) $(CFLAGS) dnaio.c

job.o: job.c job.h algo.h
	$(CC) $(CFLAGS) job.c

thread.o: thread.c algo.h dnaio.h job.h
	$(CC) $(CFLAGS) thread.c

gendna: gendna.o
	$(CC) gendna.o -o gendna

gendna.o: gendna.c
	$(CC) $(CFLAGS) gendna.c

clean:
	rm -rf *.o gendna dna

