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

#include <time.h>

// Comparison function for integers
int compare_int(const void *a, const void *b) {
    int int_a = *((int*)a);
    int int_b = *((int*)b);
    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

// Print function for integer keys
void print_int(const void *data) {
    printf("%d", *((int*)data));
}

// Print function for string data
void print_string(const void *data) {
    printf("%s", (char*)data);
}

// Free function for dynamic data
void free_dynamic(void *data) {
    free(data);
}

void test_b_tree_random_big(int min_degree, int num_elements, int key_range) {
    printf("=== Testing B-Tree (t=%d) with %d Random Elements ===\n", min_degree, num_elements);

    BTree *tree = create_tree(min_degree, compare_int, print_int, print_int, free_dynamic, free_dynamic);
    
    srand(time(NULL)); 
    printf("Inserting %d random elements (Keys 0-%d):\n", num_elements, key_range);
    
    int *inserted_keys = (int*)malloc(sizeof(int) * num_elements);
    
    for (int i = 0; i < num_elements; i++) {
        int *key = (int*)malloc(sizeof(int));
        int *data = (int*)malloc(sizeof(int));
        
        *key = rand() % key_range; // Random keys
        *data = *key * 10; // Data is key * 10 for easy verification
        
        insert(tree, data, key);
        
        inserted_keys[i] = *key;
        if (i < 5 || i >= num_elements - 5) {
            printf("  Inserted key: %d\n", *key);
        } else if (i == 5) {
            printf("  ...\n");
        }
    }
    
    printf("\nTree structure after insertions (t=%d):\n", min_degree);
    display(tree);
    
    // Test search on some inserted and random keys
    printf("\n=== Testing Search on B-Tree (t=%d) ===\n", min_degree);
    for (int i = 0; i < 10; i++) {
        int search_key;
        
        if (i < 5) {
            // Search for an inserted key (e.g., from the first few)
            search_key = inserted_keys[rand() % num_elements];
            printf("Searching for inserted key: %d", search_key);
        } else {
            // Search for a random key (may or may not be present)
            search_key = rand() % key_range;
            printf("Searching for random key: %d", search_key);
        }

        void **result = search(tree, &search_key);
        if (result && *result) {
            printf(" -> Found (Data: %d)\n", *((int*)*result));
        } else {
            printf(" -> Not Found\n");
        }
    }
    
    printf("\nFinal Tree size: %d\n", tree->size);
    
    free_tree(tree); // Assume free_tree works for BTree
    free(inserted_keys);
    printf("B-Tree (t=%d) test freed successfully.\n\n", min_degree);
}

int main() {
    // Set a constant minimum degree (t). Common values are 2, 3, or 4.
    const int T_SMALL = 2;   // t=2 is a 2-3-4 tree (max 3 keys)
    const int T_MEDIUM = 4;  // t=4 (max 7 keys)
    
    // Configuration for the big random test
    const int NUM_RECORDS = 75;
    const int KEY_RANGE = 200;

    printf("B-Tree Implementation Test\n");
    printf("==========================\n\n");
    
    // Test a small B-Tree (equivalent to 2-3 tree complexity)
    test_b_tree_random_big(T_SMALL, NUM_RECORDS / 2, KEY_RANGE / 2);

    // Test a larger, wider B-Tree
    test_b_tree_random_big(T_MEDIUM, NUM_RECORDS, KEY_RANGE);
    
    printf("All B-Tree tests completed!\n");
    return 0;
}