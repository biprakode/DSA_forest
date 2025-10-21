#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef struct node_struct {
    void *data;
    struct node_struct *left;
    struct node_struct *right;
    int height; 
} node;

typedef struct tree_struct {
    int size;
    int (*compare)(const void *data1, const void *data2);
    void (*print_func)(const void *data);
    node *root;
} tree;

tree *create_tree(int (*compare_func)(const void *, const void *), void (*print_func)(const void *));
tree *build_tree_from_array(void **data, int size, int (*compare_func)(const void *, const void *), void (*print_func)(const void *));
void insert(tree *tree_obj, void *data);
bool search(tree *tree_obj, const void *data);
void delete_tree(tree *tree_obj, void *data);
void display(tree *tree_obj);
void **preorder(tree *tree_obj);
void **inorder(tree *tree_obj);
void **postorder(tree *tree_obj);
void free_tree(tree *tree_obj);
bool is_empty(const tree *tree_obj);
int get_height(node *node_obj);
void update_height(node *node_obj);
int get_size_tree(const tree *tree_obj);
node *insert_node(tree *tree_obj, node *current, void *data_obj);
node *search_node(tree *tree_obj, node *current, const void *data_obj);
node *delete_node_recursive(tree *tree_obj, node *current, const void *data_obj, bool *deleted);
node *find_min(node *current);
void display_tree_recursive(tree *tree_obj, node *parent, const char *prefix, bool is_left, int depth);
void preorder_recursive(node *current, void **nodes, int *i);
void inorder_recursive(node *current, void **nodes, int *i);
void postorder_recursive(node *current, void **nodes, int *i);
void free_nodes_recursive(node *current);

#endif