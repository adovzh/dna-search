CC=gcc
override CFLAGS+=-c -Wall

all: dna

dna: dna.o
	$(CC) $(LDFLAGS) dna.o -o dna

dna.o: dna.c
	$(CC) $(CFLAGS) dna.c

gendna: gendna.o
	$(CC) gendna.o -o gendna

gendna.o: gendna.c
	$(CC) $(CFLAGS) gendna.c

clean:
	rm -rf *.o gendna dna

