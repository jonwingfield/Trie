CFLAGS=-Wall -std=gnu99 -g -DNDEBUG -pg

all: test

test: trie.o

clean: 
	rm -f test trie.o
