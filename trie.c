#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"
#include "dbg.h"

#define DEFAULT_CHILDREN 4

// Trie_typ** g_node_pool = calloc(100000, sizeof(Trie_typ));
// int g_node_pool_idx = 0;

typedef struct Trie_data {
    int size;
    char val;
    int num_children;
    int children_size;
} Trie_data_typ;

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

void Get_node(Trie_typ* node, FILE* file)
{
	int size = sizeof(Trie_data_typ);

    if (node->string_value) 
        size += strlen(node->string_value) + 1;

    char* data = malloc(size); 
    Trie_data_typ* trie_data = (Trie_data_typ*)data;
    trie_data->size = size;
    trie_data->val = node->val;
    trie_data->num_children = node->num_children;
    trie_data->children_size = node->children_size;
    if (node->string_value) {
        strcpy(data + sizeof(Trie_data_typ), node->string_value);
        data[size-1] = '\0';
    }
    
    fwrite(data, sizeof(char), size, file);
}

void Trie_save(Trie_typ* trie, FILE* file)
{
    Get_node(trie, file);	
    for (int i=0; i<trie->num_children; i++) {
        Trie_save(trie->children[i], file);        
    }				
}

Trie_typ* Trie_load(FILE* file)
{
    Trie_data_typ trie_data;
    if (!fread(&trie_data, sizeof(trie_data), 1, file)) {
        printf("Error occurred while reading\n");
        return NULL;
    }

    if (trie_data.size < 1) {
        printf("Encountered error reading trie!\n");
        return NULL;
    }

    debug("Reading entry of size %d bytes", trie_data.size);

    Trie_typ* node = malloc(sizeof(Trie_typ));
    node->val = trie_data.val;
    node->num_children = trie_data.num_children;
    node->children_size = trie_data.children_size;

    debug("Loaded node for '%c'", node->val); 

    if (trie_data.size > sizeof(Trie_data_typ)) {
        size_t string_size = trie_data.size - sizeof(Trie_data_typ);
        node->string_value = malloc(string_size);
        fread(node->string_value, sizeof(char), string_size, file);
        debug("read text value %s", node->string_value);
    }

    debug("Allocating %d children", node->children_size); 
    node->children = malloc(sizeof(Trie_typ*) * node->children_size);
    
    for (int i=0; i<node->num_children; i++) {
        node->children[i] = Trie_load(file);
    }
    
    return node;
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
