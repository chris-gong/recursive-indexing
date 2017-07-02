CC = gcc
CFLAGS = -g -Wall

all: redblack linkedlist compare invertedIndex

redblack: src/redblack.c
	$(CC) $(CFLAGS) -c src/redblack.c

linkedlist: src/linkedlist.c
	$(CC) $(CFLAGS) -c src/linkedlist.c

compare: src/comparestrings.c src/comparestrings.h
	$(CC) $(CFLAGS) -c src/comparestrings.c

invertedIndex: src/invertedIndex.c redblack.o linkedlist.o
	$(CC) $(CFLAGS) -o indexer src/invertedIndex.c redblack.o linkedlist.o comparestrings.o

clean:
	rm -f indexer redblack.o linkedlist.o comparestrings.o
