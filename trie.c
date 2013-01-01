#include <stdlib.h>
#include <string.h>
#include "trie.h"
#include "dbg.h"

#define DEFAULT_CHILDREN 4

Trie_typ* Trie_init()
{
	Trie_typ* root = calloc(1, sizeof(Trie_typ));

	root->children_size = DEFAULT_CHILDREN;
	root->children = calloc(DEFAULT_CHILDREN, sizeof(Trie_typ*));

	return root;
}

void Trie_destroy(Trie_typ* trie)
{
	// do a depth-first search and cleanup all nodes
	for (int i = 0; i < trie->num_children; ++i)
	{
		Trie_destroy(trie->children[i]);
	}

	if (trie->string_value) free(trie->string_value);
	trie->string_value = NULL;
	free(trie->children);
	free(trie);
}

int Trie_add_node_to_children(Trie_typ* parent, Trie_typ* child)
{
	if (parent->num_children == parent->children_size) {
		parent->children_size += 8;
		debug("Resizing children for %c to %d", parent->val, parent->children_size);
		parent->children = realloc(parent->children, 
			parent->children_size * sizeof(Trie_typ*));
	}

	parent->children[parent->num_children++] = child;

	return 0;
// error:
// 	return 1;
}

Trie_typ* Trie_find_node(Trie_typ* node, char item)
{
	Trie_typ* found = NULL;

	for (int i = 0; i < node->num_children; ++i)
	{
		if (node->children[i]->val == item) {
			found = node->children[i];
			break;
		}
	}

	return found;
}

Trie_typ* Trie_add_node(Trie_typ* node, char item)
{
	Trie_typ* found = Trie_find_node(node, item);

	if (!found) {
		debug("Adding new node %c to parent %c", item, node->val);
		found = Trie_init();
		found->val = item;
		Trie_add_node_to_children(node, found);
	}

	return found;
}

int Trie_add(Trie_typ* trie, char* item)
{
	Trie_typ* cur_node = trie;

	debug("%s", item);

	for (char* letter = item; *letter; ++letter)
	{
		cur_node = Trie_add_node(cur_node, *letter);
	}

	cur_node->string_value = strdup(item);

	return 0;
}

StringNode_typ* add_results(Trie_typ* trie, StringNode_typ* list)
{
	for (int i = 0; i < trie->num_children; ++i)
	{
		list = add_results(trie->children[i], list);
	}

	if (trie->string_value) {
		list->value = trie->string_value;
		list->next = calloc(1, sizeof(StringNode_typ));
		list = list->next;
	}

	return list;
}

StringNode_typ* Trie_find(Trie_typ* trie, char* item)
{
	Trie_typ* cur_node = trie;

	for (char* letter = item; *letter; ++letter)
	{
		cur_node = Trie_find_node(cur_node, *letter);
		if (!cur_node) return NULL;
	}

	StringNode_typ* list = calloc(1, sizeof(StringNode_typ));
	add_results(cur_node, list);
	return list;
}

void StringNode_free(StringNode_typ* nodes)
{
	StringNode_typ* next = NULL;
	while (nodes)
	{
		next = nodes->next;
		free(nodes);
		nodes = next;
	}
}