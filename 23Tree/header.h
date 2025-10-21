#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct node23_struct {
    void **keys;
    void **data;
    struct node23_struct **children;
    int key_count;
    bool is_leaf;
    struct node23_struct *parent;
} Node23;

typedef struct tree23_struct {
    int size;
    int (*compare)(const void *, const void *);
    void (*print_data)(const void *);
    void (*print_key)(const void *);
    void (*free_data)(void *); 
    void (*free_key)(void *);
    Node23 *root;
} Tree23;

Tree23 *create_tree(int (*compare_func)(const void *, const void *), 
                   void (*print_key)(const void *),
                   void (*print_data)(const void *),
                   void (*free_data)(void *),
                   void (*free_key)(void *));
void free_tree(Tree23 *tree_obj);

Node23 *node_create(bool is_leaf);
void node_destroy(Node23 *node23 , Tree23 *tree);
bool is_empty(Tree23 *tree);
void insert(Tree23 *tree, void *data, void *key);
void** search(Tree23 *tree, const void *key);
Node23 *recursive_search(Tree23 *tree, Node23 *node, const void *key);
Node23 *insert_recursive(Tree23 *tree, Node23 *current, void *key, void *data, void **key_up, void **data_up);
void insert_key_data(Node23 *node, void *key, void *data, Node23 *child, int pos);
int find_child_index(Tree23 *tree, Node23 *current, void *key);
Node23 *split_node(Tree23 *tree, Node23 *old_node, void *new_key, void *new_data, Node23 *new_child, void **median_key, void **median_data);
void realign_children(Node23 *node, int pos, Node23 *child_node);
void display(Tree23 *tree_obj);
void display_tree_recursive(Tree23 *tree, Node23 *node, const char *prefix, int depth);