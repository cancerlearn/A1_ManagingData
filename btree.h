/*
 * 
 * You will need to write your B+Tree almost entirely from scratch. 
 * 
 * B+Trees are dynamically balanced tree structures that provides efficient support for insertion, deletion, equality, and range searches. 
 * The internal nodes of the tree direct the search and the leaf nodes hold the base data..
 * 
 * For a basic rundown on B+Trees, we will refer to parts of Chapter 10 of the textbook Ramikrishnan-Gehrke 
 * (all chapters and page numbers in this assignment prompt refer to the 3rd edition of the textbook).
 *
 * Read Chapter 10 which is on Tree Indexing in general. In particular, focus on Chapter 10.3 on B+Tree.
 */

#ifndef BTREE_H
#define BTREE_H

#define MAX_KEYS (1024)
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "query.h"
#include "btree_definitions.h"


/* 
Designing your C Structs for B+Tree nodes (Chapter 10.3.1)
How will you represent a B+Tree as a C Struct (or series of C structs that work together)? There are many valid ways to do this part of your design, and we leave it open to you to try and tune this as you progress through the project.
How will you account for a B+Tree node being an internal node or a leaf node? Will you have a single node type that can conditionally be either of the two types, or will you have two distinct struct types?
How many children does each internal node have? This is called the fanout of the B+Tree.
What is the maximum size for a leaf node? How about for an internal node?
What is the minimum threshold of content for a node, before it has to be part of a rebalancing?
*/

// TODO: here you will need to define a B+Tree node(s) struct(s)

struct Node
{
	int isLeaf;
	int numKeys;
	Key_Val kv_arr[MAX_KEYS];
	struct Node* pointers[MAX_KEYS + 1];
};

BP_Tree BPT_Create(void)
{
	BP_Tree bptree;

	bptree = malloc(sizeof(*bptree));
	assert(bptree);

	bptree->isLeaf = 1;
	bptree->numKeys = 0;

	return bptree;
}

void print_tree(BP_Tree bptree)
{
	for (int i = 0; i < bptree->numKeys; i++)
	{
		printf("%d ", bptree->kv_arr[i]);
	}
	printf("\n");
	for (int j = 0; j < bptree->numKeys; j++)
	{
		print_tree(bptree->pointers[j]);
	}
	return;
}

/* FIND (Chapter 10.4)
This is an equality search for an entry whose key matches the target key exactly.
How many nodes need to be accessed during an equality search for a key, within the B+Tree? 
*/

int find(BP_Tree bptree, Key_Val key)
{
	int pos;

	// if the b+tree is empty, return 0 for a 'no'
	if (bptree->numKeys == 0)
	{
		return 0;
	}

	// get the valid position for the key
	pos = find_key(bptree->numKeys, bptree->kv_arr, key);

	// if the key already exists, return 1 to signify a 'yes'
	if (pos < bptree->numKeys && bptree->kv_arr[pos] == key)
	{
		return 1;
	}
	else
	{
		return (!bptree->isLeaf && find(bptree->pointers[pos], key));
	}
}

// TODO: here you will need to define FIND/SEARCH related method(s) of finding key-values in your B+Tree.

Key_Val find_key(int n, const int *a, Key_Val key)
{
	int lo;
  	int hi;
	int mid;

	lo = -1;
	hi = n;

	while (lo + 1 < hi)
	{
		mid = (lo + hi) / 2;
		if (a[mid] == key)
		{
			return mid;
		}
		else if (a[mid] < key)
		{
			lo = mid;
		}
		else
		{
			hi = mid;
		}
	}

	return hi;
}
/* INSERT (Chapter 10.5)
How does inserting an entry into the tree differ from finding an entry in the tree?
When you insert a key-value pair into the tree, what happens if there is no space in the leaf node? What is the overflow handling algorithm?
For Splitting B+Tree Nodes (Chapter 10.8.3)
*/
void insert(BP_Tree bptree, Key_Val key)
{
	BP_Tree sub_tree1;
	BP_Tree sub_tree2;
	int median;

	sub_tree2 = internal_insert(bptree, key, &median);

	if (sub_tree2)
	{
		sub_tree1 = malloc(sizeof(*sub_tree1));
		assert(sub_tree1);

		memmove(sub_tree1, bptree, sizeof(*bptree));

		bptree->numKeys = 1;
		bptree->isLeaf = 0;
		bptree->kv_arr[0] = median;
		bptree->pointers[0] = sub_tree1;
		bptree->pointers[1] = sub_tree2;
		// print_tree(sub_tree2);
	}
}

// TODO: here you will need to define INSERT related method(s) of adding key-values in your B+Tree.

BP_Tree internal_insert(BP_Tree bptree, Key_Val key, int* median)
{
	int pos;
	int mid;	/* mid index node would be split at */
	BP_Tree sub_tree;

	pos = find_key(bptree->numKeys, bptree->kv_arr, key);

	if (pos < bptree->numKeys && bptree->kv_arr[pos] == key)
	{
		// return 0, since key is already in the tree
		return 0;
	}

	if (bptree->isLeaf)
	{
		// move all values in kv_arr above the insertion postion by 1
		memmove(&bptree->kv_arr[pos + 1], &bptree->kv_arr[pos], sizeof(*(bptree->kv_arr)) * (bptree->numKeys - pos));
		bptree->kv_arr[pos] = key;
		bptree->numKeys++;
	}
	else
	{
		sub_tree = internal_insert(bptree->pointers[pos], key, &mid);

		if (sub_tree)
		{
			// Moving keys up by one position to make space for new key
			memmove(&bptree->kv_arr[pos + 1], &bptree->kv_arr[pos], sizeof(*(bptree->kv_arr)) * (bptree->numKeys - pos));
			// Moving pointers to children nodes by one position up to make space for new child node (sub_tree)
			memmove(&bptree->pointers[pos+2], &bptree->pointers[pos+1], sizeof(*(bptree->kv_arr)) * (bptree->numKeys - pos));

			bptree->kv_arr[pos] = mid;
			bptree->pointers[pos + 1] = sub_tree;
			bptree->numKeys++;
		}
	}

	if (bptree->numKeys >= MAX_KEYS)
	{
		mid = bptree->numKeys/2;
		*median = bptree->kv_arr[mid];	/* value from the mid index */

		// Allocating memory for new node originating from splitting
		// and setting its properties.
		sub_tree = malloc(sizeof(*sub_tree) + sizeof(bptree)/sizeof(bptree->kv_arr[0]));
		sub_tree->numKeys = bptree->numKeys - mid - 1;
		sub_tree->isLeaf = bptree->isLeaf;

		// Transferring keys from parent to new split node.
		memmove(sub_tree->kv_arr, &bptree->kv_arr[mid + 1], sizeof(*(bptree->kv_arr)) * sub_tree->numKeys);
		// Transferring corresponding child nodes if the parent node (node being split)
		// was not a leaf node.
		if (!bptree->isLeaf)
		{
			memmove(sub_tree->pointers, &bptree->pointers[mid + 1], sizeof(*(bptree->pointers)) * (sub_tree->numKeys + 1));
		}

		bptree->numKeys = mid;

		// Keeping value from parent in leaf node. A B+tree property.
		int temp_pos = find_key(sub_tree->numKeys, sub_tree->kv_arr, bptree->kv_arr[mid]);	// Obtain valid (to keep keys sorted) postion for parent's value
		// Moving every key up by one
        memmove(&sub_tree->kv_arr[temp_pos + 1], &sub_tree->kv_arr[temp_pos], sizeof(*(sub_tree->kv_arr)) * (sub_tree->numKeys - temp_pos));
        // Inserting value from parent
		sub_tree->kv_arr[temp_pos] = *median;
		
		return sub_tree;
	}
	else
	{
		return 0;
	}	
}


/* BULK LOAD (Chapter 10.8.2)
Bulk Load is a special operation to build a B+Tree from scratch, from the bottom up, when beginning with an already known dataset.
Why might you use Bulk Load instead of a series of inserts for populating a B+Tree? Compare the cost of a Bulk Load of N data entries versus that of an insertion of N data entries? What are the tradeoffs?
*/

// TODO: here you will need to define BULK LOAD related method(s) of initially adding all at once some key-values to your B+Tree.
// BULK LOAD only can happen at the start of a workload


/*RANGE (GRADUATE CREDIT)
Scans are range searches for entries whose keys fall between a low key and high key.
Consider how many nodes need to be accessed during a range search for keys, within the B+Tree?
Can you describe two different methods to return the qualifying keys for a range search? 
(Hint: how does the algorithm of a range search compare to an equality search? What are their similarities, what is different?)
Can you describe a generic cost expression for Scan, measured in number of random accesses, with respect to the depth of the tree?
*/

// TODO GRADUATE: here you will need to define RANGE for finding qualifying keys and values that fall in a key range.


#endif