#ifndef __trie_h
#define __trie_h

typedef struct StringNode
{
	char* value;
	struct StringNode* next;
} StringNode_typ;

#define iterate(R) for (; R; R = R->next) 

void StringNode_free(StringNode_typ* nodes);

typedef struct Trie
{
	char val;
	char* string_value;
	struct Trie** children;

	int num_children;
	int children_size;
} Trie_typ;

Trie_typ* Trie_init();
void Trie_destroy(Trie_typ* trie);
int Trie_add(Trie_typ* trie, char* item);
void Trie_save(Trie_typ* trie, FILE* file); 
Trie_typ* Trie_load(FILE* file);
StringNode_typ* Trie_find(Trie_typ* trie, char* item);

#endif

