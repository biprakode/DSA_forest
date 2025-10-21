#include "header.h"

tree *create_tree(int (*compare_func)(const void *, const void *), void (*print_func)(const void *));
tree *build_tree_from_array(void **data, int size, int (*compare_func)(const void *, const void *), void (*print_func)(const void *));
void insert(tree *tree_obj, void *data); // TO MODIFY
bool search(tree *tree_obj, const void *data);
void delete_tree(tree *tree_obj, void *data); // TO MODIFY
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
int get_balance_factor(node *current);
node *rotate_left(node *current);
node *rotate_right(node *current);

int get_balance_factor(node *current) {
    int left = current->left ? get_height(current->left) : 0;
    int right = current->right ? get_height(current->right) : 0;
    return left - right; // LEFT PRIOR
}

node *rotate_right(node *current) {
    node *B = current->left;
    current->left = B->right;
    B->right = current;
    update_height(current);
    update_height(B);
    return B;
}

node *rotate_left(node *current) {
    node *B = current->right;
    current->right = B->left;
    B->left = current;
    update_height(current);
    update_height(B);
    return B;
}

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

    int bf = get_balance_factor(current);
    if(bf > 1) { // left heavy
        if(get_balance_factor(current->left) >= 0) {
            return rotate_right(current);
        }else{
            current->left = rotate_left(current->left);
            return rotate_right(current);
        }
    }else if(bf < -1) { // right heavy
        if(get_balance_factor(current->right) <= 0) {
            return rotate_left(current);
        }else{
            current->right = rotate_right(current->right);
            return rotate_left(current);
        }
    }

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
        tree_obj->size--;
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
    if (!current) { // base_case
        return NULL; 
    }

    update_height(current);
    int bf = get_balance_factor(current);
    if(bf > -1) { // left heavy
        if(get_balance_factor(current->left) >= 0) {
            return rotate_right(current);
        }else{
            current->left = rotate_left(current->left);
            return rotate_right(current);
        }
    }else if(bf < 1) { // right heavy
        if(get_balance_factor(current->right) <= 0) {
            return rotate_left(current);
        }else{
            current->right = rotate_right(current->right);
            return rotate_left(current);
        }
    }
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