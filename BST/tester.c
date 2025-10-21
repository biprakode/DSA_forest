#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

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



bool is_empty(const tree *tree_obj) {
    return tree_obj->size<=0;
}

int get_height(node *node_obj) {
    if(!node_obj) return 0;
    return node_obj->height;
}

int get_size_tree(const tree *tree_obj) {
    return tree_obj->size;
}

tree *create_tree(int (*compare_func)(const void *, const void *), void (*print_func)(const void *)) {
    tree *tree_obj = (tree *)malloc(sizeof(tree));
    if (!tree_obj) {
        perror("Failed to allocate memory for tree");
        return NULL;
    }
    tree_obj->root = NULL;
    tree_obj->size = 0;
    tree_obj->compare = compare_func;
    tree_obj->print_func = print_func;
    return tree_obj;
}

tree *build_tree_from_array(void **data, int size, int (*compare_func)(const void *, const void *), void (*print_func)(const void *)) {
    tree *tree_obj = create_tree(compare_func, print_func);
    if (!tree_obj) return NULL;
    
    for (int i = 0; i < size; i++) {
        insert(tree_obj, data[i]);
    }
    return tree_obj;
}

void insert(tree *tree_obj , void *data) {
    tree_obj->root = insert_node(tree_obj, tree_obj->root, data);
}

void update_height(node *node_obj) {
    if (node_obj) {
        node_obj->height = 1 + fmax(get_height(node_obj->left), get_height(node_obj->right));
    }
}

node *insert_node(tree *tree_obj, node *current, void *data_obj) {
    if (!current) {
        node *new_node = (node *)malloc(sizeof(node));
        new_node->data = data_obj;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->height = 1; // Leaf node has height 1
        tree_obj->size++;
        return new_node;
    } if (tree_obj->compare(data_obj, current->data) < 0) {
        current->left = insert_node(tree_obj, current->left, data_obj);
    } else if (tree_obj->compare(data_obj, current->data) > 0) {
        current->right = insert_node(tree_obj, current->right, data_obj);
    } 
    update_height(current);
    return current;
}

bool search(tree *tree_obj, const void *data) {
    if (is_empty(tree_obj)) {
        return false;
    }
    node* result = search_node(tree_obj, tree_obj->root, data);
    if (result != NULL) {
        printf("Found data: ");
        tree_obj->print_func(result->data);
        printf("\n");
        return true;
    } else {
        printf("Data not found.\n");
        return false;
    }
}

node *search_node(tree *tree_obj, node *current, const void *data_obj) {
    if (current == NULL) {
        return NULL; // Data not found
    }
    
    int comparison = tree_obj->compare(data_obj, current->data);
    
    if (comparison == 0) {
        return current;
    } else if (comparison < 0) {
        return search_node(tree_obj, current->left, data_obj);
    } else {
        return search_node(tree_obj, current->right, data_obj);
    }
}

void delete_node(node *node_obj){
    free(node_obj->data);
    free(node_obj);
}

node *find_min(node *current) {
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

void delete_tree(tree *tree_obj , void *data) {
    bool deleted = false;
    tree_obj->root = delete_node_recursive(tree_obj, tree_obj->root, data, &deleted);
    if(deleted) tree_obj->size--;
}

node *delete_node_recursive(tree *tree_obj, node *current, const void *data_obj, bool *deleted) {
    if (current == NULL) {
        return NULL;
    }
    int comparison = tree_obj->compare(data_obj, current->data);
    if (comparison < 0) {
        current->left = delete_node_recursive(tree_obj, current->left, data_obj, deleted);
    } else if (comparison > 0) {
        current->right = delete_node_recursive(tree_obj, current->right, data_obj, deleted);
    } else {
        *deleted = true;
        if (current->left == NULL && current->right == NULL) {
            free(current);
            return NULL;
        }
        //1 child
        if (current->left == NULL) {
            node *temp = current->right;
            free(current);
            return temp;
        } else if (current->right == NULL) {
            node *temp = current->left;
            free(current);
            return temp;
        }

        // two children.
        node *temp = find_min(current->right);
        current->data = temp->data;
        current->right = delete_node_recursive(tree_obj, current->right, temp->data, deleted);
    }
    update_height(current);
    return current;
}

void display(tree *tree_obj) {
    if (is_empty(tree_obj) || !tree_obj->root) {
        printf("Tree is empty.\n");
        return;
    }
    char prefix[1024] = "";
    display_tree_recursive(tree_obj, tree_obj->root, prefix, false, 0);
}

void display_tree_recursive(tree *tree_obj, node *parent, const char *prefix, bool is_left, int depth) {
    if(parent == NULL) return;
    printf("%s", prefix);
    printf(is_left ? "├── " : "└── ");
    tree_obj->print_func(parent->data);
    printf(" (h:%d)\n", parent->height);

    char next_prefix[1024];
    const char *branch = is_left ? "│   " : "    ";
    if (depth * 4 + strlen(branch) + 1 < 1024) {
        strcpy(next_prefix, prefix);
        strcat(next_prefix, branch);
    } else {
        next_prefix[0] = '\0';
    }

    if (parent->left || parent->right) {
        if(parent->right) {
             display_tree_recursive(tree_obj, parent->right, next_prefix, parent->left != NULL, depth + 1);
        }
        if (parent->left) {
            display_tree_recursive(tree_obj, parent->left, next_prefix, false, depth + 1);
        }
    }
}

void **preorder(tree *tree_obj) {
    if (is_empty(tree_obj)) return NULL;
    void **nodes = (void **)malloc(sizeof(void *) * tree_obj->size);
    int i = 0;
    preorder_recursive(tree_obj->root, nodes, &i);
    return nodes;
}

void **inorder(tree *tree_obj) {
    if (is_empty(tree_obj)) return NULL;
    void **nodes = (void **)malloc(sizeof(void *) * tree_obj->size);
    int i = 0;
    inorder_recursive(tree_obj->root, nodes, &i);
    return nodes;
}

void **postorder(tree *tree_obj) {
    if (is_empty(tree_obj)) return NULL;
    void **nodes = (void **)malloc(sizeof(void *) * tree_obj->size);
    int i = 0;
    postorder_recursive(tree_obj->root, nodes, &i);
    return nodes;
}

void preorder_recursive(node *current, void **nodes, int *i) {
    if (!current) return;
    nodes[*i] = current->data;
    (*i)++;
    preorder_recursive(current->left, nodes, i);
    preorder_recursive(current->right, nodes, i);
}

void inorder_recursive(node *current, void **nodes, int *i) {
    if (!current) return;
    inorder_recursive(current->left, nodes, i);
    nodes[*i] = current->data;
    (*i)++;
    inorder_recursive(current->right, nodes, i);
}

void postorder_recursive(node *current, void **nodes, int *i) {
    if (!current) return;
    postorder_recursive(current->left, nodes, i);
    postorder_recursive(current->right, nodes, i);
    nodes[*i] = current->data;
    (*i)++;
}

void free_tree(tree *tree_obj) {
    if(tree_obj) {
        free_nodes_recursive(tree_obj->root);
        free(tree_obj);
    }
}

void free_nodes_recursive(node *current) {
    if(!current) return;
    free_nodes_recursive(current->left);
    free_nodes_recursive(current->right);
    free(current);
} 

int compare_int(const void *a, const void *b) {
    int int_a = *(int *)a;
    int int_b = *(int *)b;
    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

void print_int(const void *data) {
    printf("%d", *(int *)data);
}


int compare_double(const void *a, const void *b) {
    double double_a = *(double *)a;
    double double_b = *(double *)b;
    if (double_a < double_b) return -1;
    if (double_a > double_b) return 1;
    return 0;
}

void print_double(const void *data) {
    printf("%.2f", *(double *)data);
}

int main() {
    srand(time(NULL)); 
    printf("TESTING BINARY SEARCH TREE WITH INTEGERS\n\n");

    const int NUM_INTS = 100;
    int *int_data[NUM_INTS];
    void *generic_ptr_int[NUM_INTS];

    printf("Generating random integers: ");
    for (int i = 0; i < NUM_INTS; i++) {
        int_data[i] = (int *)malloc(sizeof(int));
        *int_data[i] = rand() % 1000;
        generic_ptr_int[i] = int_data[i];
        printf("%d ", *int_data[i]);
    }
    printf("\n\n");

    tree *int_tree = build_tree_from_array(generic_ptr_int, NUM_INTS, compare_int, print_int);

    printf("Initial tree structure (size: %d):\n", get_size_tree(int_tree));
    display(int_tree);
    printf("\n");

    printf("Inorder traversal: ");
    void **inorder_result = inorder(int_tree);
    for (int i = 0; i < get_size_tree(int_tree); i++) {
        print_int(inorder_result[i]);
        printf(" ");
    }
    printf("\n");
    free(inorder_result);

    printf("\nSearching for value %d...\n", *int_data[3]);
    search(int_tree, int_data[3]);
    int not_in_tree = 101;
    printf("Searching for value %d...\n", not_in_tree);
    search(int_tree, &not_in_tree);
    printf("\n");

    printf("Deleting node with value: %d\n", *int_data[2]);
    delete_tree(int_tree, int_data[2]);
    printf("Deleting node with value: %d\n", *int_data[5]);
    delete_tree(int_tree, int_data[5]);
    
    printf("\nTree structure after deletions (size: %d):\n", get_size_tree(int_tree));
    display(int_tree);
    printf("\n");


    printf("\nTESTING BINARY SEARCH TREE WITH DOUBLES\n\n");
    
    const int NUM_DOUBLES = 100;
    double *double_data[NUM_DOUBLES];
    void *generic_ptr_double[NUM_DOUBLES];

    printf("Generating random doubles: ");
    for (int i = 0; i < NUM_DOUBLES; i++) {
        double_data[i] = (double *)malloc(sizeof(double));
        *double_data[i] = (double)(rand() % 10000) / 100.0;
        generic_ptr_double[i] = double_data[i];
        print_double(double_data[i]);
        printf(" ");
    }
    printf("\n\n");

    tree *double_tree = build_tree_from_array(generic_ptr_double, NUM_DOUBLES, compare_double, print_double);
    printf("Double tree structure (size: %d):\n", get_size_tree(double_tree));
    display(double_tree);
    printf("\n");


    printf("Cleaning up memory\n");
    
    for (int i = 0; i < NUM_INTS; i++) {
        free(int_data[i]);
    }
    free_tree(int_tree);

    for (int i = 0; i < NUM_DOUBLES; i++) {
        free(double_data[i]);
    }
    free_tree(double_tree);

    printf("Cleanup complete.\n");

    return 0;
}
