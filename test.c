#include <stdio.h>
#include "trie.h"

#define iterate(R) for (; R; R = R->next) 

void find_and_print(Trie_typ* trie, char* to_find)
{
	StringNode_typ* results = Trie_find(trie, to_find);
	StringNode_typ* iter = results;

	iterate(iter) {
		if (iter->value) printf("%s\n", iter->value);
	}

	StringNode_free(results);
}

int main(int argc, char** argv)
{
	Trie_typ* trie = Trie_init();

	Trie_add(trie, "Hello");
	Trie_add(trie, "Haven!");
	Trie_add(trie, "Heaven");
	Trie_add(trie, "Hemen");

	find_and_print(trie, "He");

	Trie_destroy(trie);

	return 0;
}