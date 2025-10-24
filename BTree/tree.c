#include "header.h"

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

bool is_empty(BTree *tree) {
    return tree->size==0;
}

BTree *create_tree(int m, int (*compare_func)(const void *, const void *), void (*print_key)(const void *),void (*print_data)(const void *) , void (*free_data)(void *) , void (*free_key)(void *)) {
    BTree *tree_obj = (BTree *)malloc(sizeof(BTree));
    if (!tree_obj) {
        perror("Failed to allocate memory for tree");
        return NULL;
    }
    tree_obj->m = m;
    tree_obj->root = NULL;
    tree_obj->size = 0;
    tree_obj->compare = compare_func;
    tree_obj->print_key = print_key;
    tree_obj->print_data = print_data;
    tree_obj->free_data = free_data;
    tree_obj->free_key = free_key;
    return tree_obj;
}

BNode* node_create(bool is_leaf , int m) {
    BNode *node = (BNode *)malloc(sizeof(BNode));
    if (!node) {
        perror("Failed to allocate memory for BNode");
        return NULL;
    }
    node->keys = (void **)calloc(m-1, sizeof(void *));
    node->data = (void **)calloc(m-1, sizeof(void *));
    node->children = (BNode **)calloc(m, sizeof(BNode *));
    node->key_count = 0;
    node->parent = NULL;
    node->is_leaf = is_leaf;
    return node;
}

void node_destroy(BNode *BNode , BTree *tree) {
    if(!BNode) return;
    for (int i = 0; i < BNode->key_count; i++) {
        if (tree->free_key && BNode->keys[i]) {
            tree->free_key(BNode->keys[i]);
        }
        if (tree->free_data && BNode->data[i]) {
            tree->free_data(BNode->data[i]);
        }
    }
    if (!BNode->is_leaf) {
        for (int i = 0; i <= BNode->key_count; i++) {
            node_destroy(BNode->children[i], tree);
        }
    }
    free(BNode->data);
    free(BNode->keys);
    free(BNode->children);
    free(BNode);
}

void free_tree(BTree *tree_obj){
    if (!tree_obj) return;
    node_destroy(tree_obj->root, tree_obj);
    free(tree_obj);
}

void** search(BTree *tree, const void *key) {
    if(is_empty(tree)) {
        printf("Tree Empty\n");
        return NULL;
    }
    BNode *result = recursive_search(tree, tree->root, key);
    if (!result) return NULL;
    for (int i = 0; i < result->key_count; i++) {
        if (tree->compare(key, result->keys[i]) == 0) {
            return &(result->data[i]); 
        }
    }
    return NULL;
}

BNode *recursive_search(BTree *tree , BNode *node , const void *key) {
    if(!node) return NULL;
    for (int i = 0; i < node->key_count; i++) {
        if (tree->compare(key, node->keys[i]) == 0) {
            return node;
        }
    }
    if (node->is_leaf) return NULL; // reached leaf
    int i;
    for (i = 0; i < node->key_count; i++) {
        if (tree->compare(key, node->keys[i]) < 0) {
            return recursive_search(tree, node->children[i], key);
        }
    }
    return recursive_search(tree, node->children[i], key);
}

void insert_key_data(BNode *node, void *key, void *data, BNode *child, int pos) {
    for (int i = node->key_count; i > pos; i--) {
        node->keys[i] = node->keys[i-1];
        node->data[i] = node->data[i-1];
        if (!node->is_leaf) {
            node->children[i+1] = node->children[i];
    }
}
    // insert at pos
    node->keys[pos] = key; 
    node->data[pos] = data;
    if(!node->is_leaf) {
        node->children[pos+1] = child; // new child always right
        child->parent = node;
    }
    node->key_count++;
}

int find_child_index(BTree *tree,BNode *current,void *key) {
    int i;
    for (i = 0; i < current->key_count; i++) {
        if (tree->compare(key, current->keys[i]) < 0) {
            return i;
        }
    }
    return i;
}

BNode *split_node(BTree *tree, BNode *old_node, void *new_key, void *new_data, BNode *new_child, void **median_key, void **median_data) {
    // temp node container
    void **all_keys = (void **)malloc(tree->m * sizeof(void *));
    void **all_data = (void **)malloc(tree->m * sizeof(void *));
    BNode **all_children = (BNode **)malloc((tree->m+1) * sizeof(BNode *));
    
    int insert_idx = 0;
    while (insert_idx < old_node->key_count && tree->compare(new_key, old_node->keys[insert_idx]) > 0) {
        insert_idx++;
    }

    for (int i = 0 , j=0; i < tree->m; i++) {
        if (i == insert_idx) {
            all_keys[i] = new_key;
            all_data[i] = new_data;
        } else {
            all_keys[i] = old_node->keys[j];
            all_data[i] = old_node->data[j];
            j++;
        }
    }
    if(!old_node->is_leaf) {
        for (int i = 0 , j = 0; i <= tree->m; i++) {
            if (i == insert_idx + 1) {
                all_children[i] = new_child;
            } else {
                all_children[i] = old_node->children[j++];
            }
        }
    }

    int mid = tree->m / 2;

    *median_key = all_keys[mid];
    *median_data = all_data[mid];
    old_node->key_count = mid;
    for (int i = 0; i < mid; i++) {
        old_node->keys[i] = all_keys[i];
        old_node->data[i] = all_data[i];
    }
    for (int i = mid; i < tree->m-1; i++) {
        old_node->keys[i] = NULL;
        old_node->data[i] = NULL;
    }

    if (!old_node->is_leaf) {
        for (int i = 0; i <= mid; i++) {
            old_node->children[i] = all_children[i];
            if (old_node->children[i]) old_node->children[i]->parent = old_node;
        }
        for (int i = mid+1; i < tree->m; i++) {
            old_node->children[i] = NULL;
        }
    }

    BNode *new_node = node_create(old_node->is_leaf , tree->m);
    new_node->parent = old_node->parent;
    new_node->key_count = tree->m - mid - 1;
    for (int i = 0; i < new_node->key_count; i++) {
        new_node->keys[i] = all_keys[mid + 1 + i];
        new_node->data[i] = all_data[mid + 1 + i];
    }
    if (!new_node->is_leaf) {
        for (int i = 0; i <= new_node->key_count; i++) {
            new_node->children[i] = all_children[mid + 1 + i];
            if (new_node->children[i]) new_node->children[i]->parent = new_node;
        }
    }

    free(all_keys);
    free(all_data);
    free(all_children);
    return new_node;
}

void realign_children(BNode *node , int pos , BNode *child_node) {
    if (!node->is_leaf && child_node) {
        child_node->parent = node;
    }
}

BNode *insert_recursive(BTree *tree, BNode *current, void *key, void *data, void **key_up, void **data_up) {
    if (current == NULL) {
        return NULL;
    }
    int pos;
    if(current->is_leaf) {
        pos = 0;
        for (int i = 0; i < current->key_count; i++) {
            if (tree->compare(key, current->keys[i]) < 0) {
                break;
            }
            pos++;
        }   
        if(current->key_count < (tree->m - 1)) { // case 1 (not full node -> simple insert)
            insert_key_data(current , key , data , NULL , pos);
            return NULL; // no split
        } else {
            BNode *new_node = split_node(tree , current , key , data , NULL , key_up , data_up);
            return new_node; // case 2 (full node -> split)
        }
    }

    int child_idx = find_child_index(tree , current , key);
    void *mid_key = NULL; // to pass up to parent
    void *mid_data = NULL; // to pass up to parent
    BNode *split_child = insert_recursive(tree , current->children[child_idx] , key , data , &mid_key , &mid_data);
    if(split_child == NULL) {
        return NULL; // data inserted below without spliit
    }
    split_child->parent = current;
    if(current->key_count < (tree->m-1) ) { // case 3 simple insert (space found)
        pos = 0;
        for (int i = 0; i < current->key_count; i++) {
            if (tree->compare(key, current->keys[i]) < 0) {
                break;
            }
            pos++;
        } 
        insert_key_data(current , mid_key , mid_data , split_child , pos);
        realign_children(current , pos , split_child);
        return NULL; // no split
    } else {
        BNode *new_node =  split_node(tree , current , mid_key , mid_data, split_child , key_up , data_up); // case 4 (split current with split_child)
        return new_node;
    }
}

void insert(BTree *tree, void *data , void *key) {
    void *mid_key = NULL;
    void *mid_data = NULL;
    if (tree->root == NULL) {
        tree->root = node_create(true , tree->m);
        tree->root->keys[0] = key;
        tree->root->data[0] = data;
        tree->root->key_count = 1;
        tree->size++;
        return;
    }
    BNode *split_node = insert_recursive(tree, tree->root, key, data, &mid_key, &mid_data);
    if(split_node != NULL)  { // split at root (handle here)
        BNode *new_root = node_create(false , tree->m);
        new_root->keys[0] = mid_key;
        new_root->data[0] = mid_data;
        new_root->key_count = 1;
        new_root->children[0] = tree->root;
        new_root->children[1] = split_node;
        tree->root->parent = new_root;
        split_node->parent = new_root;
        tree->root = new_root;
    }
    tree->size++;
} 

void display(BTree *tree_obj) {
    if (is_empty(tree_obj) || !tree_obj->root) {
        printf("Tree is empty.\n");
        return;
    }
    printf("2-3 Tree Structure (Keys: K | D):\n");
    char prefix[1024] = "";
    display_tree_recursive(tree_obj, tree_obj->root, prefix, 0);
}

void display_tree_recursive(BTree *tree, BNode *node, const char *prefix , int depth) {
    if (node == NULL) return;
    printf("%s", prefix);
    printf("└── "); 
    printf("[");
    for (int i = 0; i < node->key_count; i++) {
        printf("K:");
        tree->print_key(node->keys[i]); 
        printf(" | D:");
        tree->print_data(node->data[i]);
        if (i < node->key_count - 1) {
            printf(" | ");
        }
    }
    printf(" (Keys:%d, %s)\n", node->key_count, node->is_leaf ? "Leaf" : "Internal");
    char next_prefix[1024];
    const char *branch = "    ";
    if (depth * 4 + strlen(branch) + 1 < 1024) {
        strcpy(next_prefix, prefix);
        strcat(next_prefix, branch);
    } else {
        next_prefix[0] = '\0';
    }
    //recurse section
    for (int i = 0; i <= node->key_count; i++) { 
        if (node->children[i] != NULL) {
            char child_prefix[1024];
            strcpy(child_prefix, prefix);
            const char *child_branch = (i < node->key_count) ? "│   " : "    ";
            strcat(child_prefix, child_branch);
            display_tree_recursive(tree, node->children[i], child_prefix, depth + 1);
        }
    }
}