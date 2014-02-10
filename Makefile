CC=gcc
override CFLAGS+=-c -Wall -DALGO_BRUTE_FORCE

all: dna

dna: dna.o algo.o needle.o
	$(CC) $(LDFLAGS) dna.o algo.o needle.o -o dna

dna.o: dna.c
	$(CC) $(CFLAGS) dna.c

algo.o: algo.c
	$(CC) $(CFLAGS) algo.c

needle.o: needle.c
	$(CC) $(CFLAGS) needle.c

gendna: gendna.o
	$(CC) gendna.o -o gendna

gendna.o: gendna.c
	$(CC) $(CFLAGS) gendna.c

clean:
	rm -rf *.o gendna dna

