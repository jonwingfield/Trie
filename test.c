#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <sys/ioctl.h>
#include "trie.h"

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
	if (!strcmp(argv[1], "load")) {
		FILE* file = fopen("trie-data", "r");
		Trie_typ* loaded = Trie_load(file);

		struct termios oldT, newT;
		ioctl(0, TCGETS, &oldT);
		newT = oldT;
		newT.c_lflag &= ~(ECHO | ICANON);
		ioctl(0, TCSETS, &newT);	

		char word[1024];
		int pos = 0;
		while (1) {
			char c;
			read(0, &c, 1);
			if (c == '/') {
				pos = 0;
				memset(word, 0, 1024);
			} else {
				word[pos++] = c;
				word[pos] = '\0';
				find_and_print(loaded, word);
			}
		}
		return 0;
	}	

	Trie_typ* trie = Trie_init();

	char word[1024];

	while (fgets(word, 1024-1, stdin)) {
		word[strlen(word)-1] = '\0';  // replace the newline
		Trie_add(trie, word);
	}

	if (!strcmp(argv[1], "build")) {
		FILE* stream = fopen("trie-data", "w");
		if (stream) {
			Trie_save(trie, stream);
			fclose(stream);	
		} else {
			printf("Error occurred\n");
		}	
	}

	find_and_print(trie, argv[1]);

	Trie_destroy(trie);

	trie = NULL;

	return 0;
}
