CFLAGS=-Wall -std=c99 -g -DNDEBUG

all: test

test: trie.o

clean: 
	rm -f test trie.o