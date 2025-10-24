#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct BNode_struct {
    void **keys;
    void **data;
    struct BNode_struct **children;
    int key_count;
    bool is_leaf;
    struct BNode_struct *parent;
} BNode;

typedef struct BTree_struct {
    int size;
    int (*compare)(const void *, const void *);
    void (*print_data)(const void *);
    void (*print_key)(const void *);
    void (*free_data)(void *); 
    void (*free_key)(void *);
    BNode *root;
    int m;
} BTree;

BTree *create_tree(int m, int (*compare_func)(const void *, const void *), void (*print_key)(const void *),void (*print_data)(const void *) , void (*free_data)(void *) , void (*free_key)(void *));
void free_tree(BTree *tree_obj);
BNode *node_create(bool is_leaf , int m);
void node_destroy(BNode *BNode , BTree *tree);
bool is_empty(BTree *tree);
void insert(BTree *tree, void *data, void *key);
void** search(BTree *tree, const void *key);
BNode *recursive_search(BTree *tree, BNode *node, const void *key);
BNode *insert_recursive(BTree *tree, BNode *current, void *key, void *data, void **key_up, void **data_up);
void insert_key_data(BNode *node, void *key, void *data, BNode *child, int pos);
int find_child_index(BTree *tree, BNode *current, void *key);
BNode *split_node(BTree *tree, BNode *old_node, void *new_key, void *new_data, BNode *new_child, void **median_key, void **median_data);
void realign_children(BNode *node, int pos, BNode *child_node);
void display(BTree *tree_obj);
void display_tree_recursive(BTree *tree, BNode *node, const char *prefix, int depth);