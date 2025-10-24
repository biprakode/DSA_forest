#include "header.h"
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
    const int T_SMALL = 4;   // t=2 is a 2-3-4 tree (max 3 keys)
    const int T_MEDIUM = 6;  // t=4 (max 7 keys)
    
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