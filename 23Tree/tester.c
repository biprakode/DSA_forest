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

Tree23 *create_tree(int (*compare_func)(const void *, const void *), void (*print_key)(const void *),void (*print_data)(const void *),void (*free_data)(void *),void (*free_key)(void *));
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

bool is_empty(Tree23 *tree) {
    return tree->size==0;
}

Tree23 *create_tree(int (*compare_func)(const void *, const void *), void (*print_key)(const void *),void (*print_data)(const void *) , void (*free_data)(void *) , void (*free_key)(void *)) {
    Tree23 *tree_obj = (Tree23 *)malloc(sizeof(Tree23));
    if (!tree_obj) {
        perror("Failed to allocate memory for tree");
        return NULL;
    }
    tree_obj->root = NULL;
    tree_obj->size = 0;
    tree_obj->compare = compare_func;
    tree_obj->print_key = print_key;
    tree_obj->print_data = print_data;
    tree_obj->free_data = free_data;
    tree_obj->free_key = free_key;
    return tree_obj;
}

Node23* node_create(bool is_leaf) {
    Node23 *node23 = (Node23 *)malloc(sizeof(Node23));
    node23->keys = (void **)calloc(2, sizeof(void *));
    node23->data = (void **)calloc(2, sizeof(void *));
    node23->children = (Node23 **)calloc(3, sizeof(Node23 *));
    node23->key_count = 0;
    node23->parent = NULL;
    node23->is_leaf = is_leaf;
    return node23;
}

void node_destroy(Node23 *node23 , Tree23 *tree) {
    if(!node23) return;
    for (int i = 0; i < node23->key_count; i++) {
        if (tree->free_key && node23->keys[i]) {
            tree->free_key(node23->keys[i]);
        }
        if (tree->free_data && node23->data[i]) {
            tree->free_data(node23->data[i]);
        }
    }
    if (!node23->is_leaf) {
        for (int i = 0; i <= node23->key_count; i++) {
            node_destroy(node23->children[i], tree);
        }
    }
    free(node23->data);
    free(node23->keys);
    free(node23->children);
    free(node23);
}

void free_tree(Tree23 *tree_obj){
    if (!tree_obj) return;
    node_destroy(tree_obj->root, tree_obj);
    free(tree_obj);
}

void** search(Tree23 *tree, const void *key) {
    if(is_empty(tree)) {
        printf("Tree Empty\n");
        return NULL;
    }
    Node23 *result = recursive_search(tree, tree->root, key);
    if (!result) return NULL;
    for (int i = 0; i < result->key_count; i++) {
        if (tree->compare(key, result->keys[i]) == 0) {
            return &(result->data[i]); 
        }
    }
    return NULL;
}

Node23 *recursive_search(Tree23 *tree , Node23 *node , const void *key) {
    if(!node) return NULL;
    for (int i = 0; i < node->key_count; i++) {
        if (tree->compare(key, node->keys[i]) == 0) {
            return node;
        }
    }
    if (node->is_leaf) return NULL; // reached leaf
    if (tree->compare(key, node->keys[0]) < 0) {
        return recursive_search(tree, node->children[0], key);
    } else if (node->key_count == 1 || tree->compare(key, node->keys[1]) < 0) {
        return recursive_search(tree, node->children[1], key);
    } else {
        return recursive_search(tree, node->children[2], key);
    }
}

void insert_key_data(Node23 *node, void *key, void *data, Node23 *child, int pos) {
    if (pos == 0 && node->key_count == 1) { // shift key & data if only 1 key/value
        node->keys[1] = node->keys[0];
        node->data[1] = node->data[0];
        if (!node->is_leaf) {
            node->children[2] = node->children[1];
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

int find_child_index(Tree23 *tree,Node23 *current,void *key) {
    if (tree->compare(key, current->keys[0]) < 0) {
        return 0;
    } else if (current->key_count == 1 || tree->compare(key, current->keys[1]) < 0) {
        return 1;
    } else {
        return 2;
    }
}

Node23 *split_node(Tree23 *tree, Node23 *old_node, void *new_key, void *new_data, Node23 *new_child, void **median_key, void **median_data) {
    // temp node container
    void *all_keys[3];
    void *all_data[3];
    Node23 *all_children[4];
    int insert_idx = 0;
    while (insert_idx < 2 && tree->compare(new_key, old_node->keys[insert_idx]) > 0) {
        insert_idx++;
    }
    int j = 0;
    for (int i = 0; i < 3; i++) {
        if (i == insert_idx) {
            all_keys[i] = new_key;
            all_data[i] = new_data;
        } else {
            all_keys[i] = old_node->keys[j];
            all_data[i] = old_node->data[j];
            j++;
        }
    }
    j = 0;
    if(!old_node->is_leaf) {
        for (int i = 0; i < 4; i++) {
            if (i != insert_idx + 1) {
                all_children[i] = old_node->children[j++];
            } else {
                all_children[i] = new_child;
            }
        }
    }

    *median_key = all_keys[1];
    *median_data = all_data[1];
    old_node->keys[0] = all_keys[0];
    old_node->data[0] = all_data[0];
    old_node->keys[1] = NULL;
    old_node->data[1] = NULL;
    old_node->key_count = 1;
    if (!old_node->is_leaf) {
        old_node->children[0] = all_children[0];
        old_node->children[1] = all_children[1];
        old_node->children[2] = NULL;
        if (old_node->children[0]) old_node->children[0]->parent = old_node;
        if (old_node->children[1]) old_node->children[1]->parent = old_node;
    }

    Node23 *new_node = node_create(old_node->is_leaf);
    new_node->parent = old_node->parent;
    new_node->keys[0] = all_keys[2];
    new_node->data[0] = all_data[2];
    new_node->key_count = 1;
    if (!new_node->is_leaf) {
        new_node->children[0] = all_children[2];
        new_node->children[1] = all_children[3];
        if (new_node->children[0]) new_node->children[0]->parent = new_node;
        if (new_node->children[1]) new_node->children[1]->parent = new_node;
    }

    return new_node;
}

void realign_children(Node23 *node , int pos , Node23 *child_node) {
    if (!node->is_leaf && child_node) {
        child_node->parent = node;
    }
}

Node23 *insert_recursive(Tree23 *tree, Node23 *current, void *key, void *data, void **key_up, void **data_up) {
    if (current == NULL) {
        return NULL;
    }
    int pos;
    if(current->is_leaf) {
        pos = tree->compare(key , current->keys[0]) < 0 ? 0 :current->key_count;
        if(current->key_count < 2) { // case 1 (not full node -> simple insert)
            insert_key_data(current , key , data , NULL , pos);
            return NULL; // no split
        } else {
            Node23 *new_node = split_node(tree , current , key , data , NULL , key_up , data_up);
            return new_node; // case 2 (full node -> split)
        }
    }

    int child_idx = find_child_index(tree , current , key);
    void *mid_key = NULL; // to pass up to parent
    void *mid_data = NULL; // to pass up to parent
    Node23 *split_child = insert_recursive(tree , current->children[child_idx] , key , data , &mid_key , &mid_data);
    if(split_child == NULL) {
        return NULL; // data inserted below without spliit
    }
    split_child->parent = current;
    if(current->key_count < 2) {
        pos = tree->compare(mid_key , current->keys[0]) < 0 ? 0 :current->key_count; // case 3 simple insert (space found)
        insert_key_data(current , mid_key , mid_data , split_child , pos);
        realign_children(current , pos , split_child);
        return NULL; // no split
    } else {
        Node23 *new_node =  split_node(tree , current , mid_key , mid_data, split_child , key_up , data_up); // case 4 (split current with split_child)
        return new_node;
    }
}

void insert(Tree23 *tree, void *data , void *key) {
    void *mid_key = NULL;
    void *mid_data = NULL;
    if (tree->root == NULL) {
        tree->root = node_create(true);
        tree->root->keys[0] = key;
        tree->root->data[0] = data;
        tree->root->key_count = 1;
        tree->size++;
        return;
    }
    Node23 *split_node = insert_recursive(tree, tree->root, key, data, &mid_key, &mid_data);
    if(split_node != NULL)  { // split at root (handle here)
        Node23 *new_root = node_create(false);
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

void display(Tree23 *tree_obj) {
    if (is_empty(tree_obj) || !tree_obj->root) {
        printf("Tree is empty.\n");
        return;
    }
    printf("2-3 Tree Structure (Keys: K | D):\n");
    char prefix[1024] = "";
    display_tree_recursive(tree_obj, tree_obj->root, prefix, 0);
}

void display_tree_recursive(Tree23 *tree, Node23 *node, const char *prefix , int depth) {
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

// Test with integer keys and string data
void test_int_string() {
    printf("=== Testing 2-3 Tree with Integer Keys and String Data ===\n");
    
    Tree23 *tree = create_tree(compare_int, print_int, print_string, free_dynamic, free_dynamic);
    
    // Insert some test data
    char *test_data[] = {"Alice", "Bob", "Charlie", "David", "Eve", "Frank", "Grace", "Henry"};
    int num_test = sizeof(test_data) / sizeof(test_data[0]);
    
    printf("Inserting %d elements:\n", num_test);
    for (int i = 0; i < num_test; i++) {
        int *key = (int *)malloc(sizeof(int));
        *key = (i + 1) * 10; // 10, 20, 30, ...
        char *data = (char *)malloc(strlen(test_data[i]) + 1);
        strcpy(data, test_data[i]);
        
        insert(tree, data, key);
        printf("Inserted key: %d, data: %s\n", *key, data);
    }
    
    printf("\nTree structure after insertions:\n");
    display(tree);
    
    // Test search
    printf("\n=== Testing Search ===\n");
    int search_keys[] = {10, 30, 50, 100};
    int num_searches = sizeof(search_keys) / sizeof(search_keys[0]);
    
    for (int i = 0; i < num_searches; i++) {
        void **result = search(tree, &search_keys[i]);
        if (result && *result) {
            printf("Found key %d: ", search_keys[i]);
            print_string(*result);
            printf("\n");
        } else {
            printf("Key %d not found\n", search_keys[i]);
        }
    }
    
    printf("\nTree size: %d\n", tree->size);
    printf("Is tree empty? %s\n", is_empty(tree) ? "Yes" : "No");
    
    free_tree(tree);
    printf("Tree freed successfully.\n\n");
}

// Test with random data
void test_random_data() {
    printf("=== Testing 2-3 Tree with Random Data ===\n");
    
    Tree23 *tree = create_tree(compare_int, print_int, print_int, free_dynamic, free_dynamic);
    
    srand(time(NULL));
    int num_elements = 15;
    
    printf("Inserting %d random elements:\n", num_elements);
    for (int i = 0; i < num_elements; i++) {
        int *key = (int *)malloc(sizeof(int));
        int *data = (int *)malloc(sizeof(int));
        
        *key = rand() % 100; // Random keys 0-99
        *data = rand() % 1000; // Random data 0-999
        
        insert(tree, data, key);
        printf("Inserted key: %d, data: %d\n", *key, *data);
    }
    
    printf("\nTree structure with random data:\n");
    display(tree);
    
    // Test some searches
    printf("\n=== Testing Random Searches ===\n");
    for (int i = 0; i < 5; i++) {
        int search_key = rand() % 100;
        void **result = search(tree, &search_key);
        if (result && *result) {
            printf("Found key %d: data = ", search_key);
            print_int(*result);
            printf("\n");
        } else {
            printf("Key %d not found\n", search_key);
        }
    }
    
    printf("\nTree size: %d\n", tree->size);
    
    free_tree(tree);
    printf("Tree with random data freed successfully.\n\n");
}

// Test edge cases
void test_edge_cases() {
    printf("=== Testing Edge Cases ===\n");
    
    Tree23 *tree = create_tree(compare_int, print_int, print_int, NULL, NULL);
    
    // Test empty tree
    printf("Testing empty tree:\n");
    printf("Is tree empty? %s\n", is_empty(tree) ? "Yes" : "No");
    display(tree);
    
    // Test single element
    printf("\nAdding single element:\n");
    int key1 = 42, data1 = 100;
    insert(tree, &data1, &key1);
    display(tree);
    
    // Test duplicate keys (should work if compare returns equal)
    printf("\nAdding element with same key:\n");
    int data2 = 200;
    insert(tree, &data2, &key1);
    display(tree);
    
    // Test search on single element
    void **result = search(tree, &key1);
    if (result && *result) {
        printf("Search for key %d found: ", key1);
        print_int(*result);
        printf("\n");
    }
    
    printf("Tree size: %d\n", tree->size);
    
    // Don't free static data to avoid double free
    tree->free_data = NULL;
    tree->free_key = NULL;
    free_tree(tree);
    printf("Edge case testing completed.\n\n");
}

// Test sequential insertion
void test_sequential() {
    printf("=== Testing Sequential Insertion ===\n");
    
    Tree23 *tree = create_tree(compare_int, print_int, print_int, free_dynamic, free_dynamic);
    
    printf("Inserting sequential keys:\n");
    for (int i = 0; i < 10; i++) {
        int *key = (int *)malloc(sizeof(int));
        int *data = (int *)malloc(sizeof(int));
        *key = i * 5; // 0, 5, 10, 15, ...
        *data = i * 100;
        
        insert(tree, data, key);
        printf("Inserted key: %d\n", *key);
    }
    
    printf("\nTree structure with sequential keys:\n");
    display(tree);
    
    printf("\nTree size: %d\n", tree->size);
    
    free_tree(tree);
    printf("Sequential test completed.\n\n");
}

int main() {
    printf("2-3 Tree Implementation Test\n");
    printf("============================\n\n");
    
    test_int_string();
    test_random_data();
    test_sequential();
    test_edge_cases();
    
    printf("All tests completed successfully!\n");
    return 0;
}