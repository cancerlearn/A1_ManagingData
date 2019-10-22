#ifndef BTREE_DEFINITIONS
#define BTREE_DEFINITIONS

#include "data_types.h"

typedef struct Node *BP_Tree;
typedef KEY_t Key_Val;
typedef VAL_t Key_Val;

// Used to create a new b+tree
BP_Tree BPT_Create(void);

// Used to print b+tree
void print_tree(BP_Tree bptree);

// Searches for a key in the b+tree
int find(BP_Tree bptree, Key_Val key);

// Provides valid position for key to be inserted 
Key_Val find_key(int n, const int *a, Key_Val key);

// Inserts a key-value into the b+tree
void insert(BP_Tree bptree, Key_Val key);

//
BP_Tree internal_insert(BP_Tree bptree, Key_Val key, int* median);

#endif