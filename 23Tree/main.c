#include "header.h"
#include <time.h>

int compare_int(const void *a, const void *b) {
    int int_a = *((int*)a);
    int int_b = *((int*)b);
    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

void print_int(const void *data) {
    printf("%d", *((int*)data));
}

void print_string(const void *data) {
    printf("%s", (char*)data);
}

void free_dynamic(void *data) {
    free(data);
}

void test_int_string() {
    printf("=== Testing 2-3 Tree with Integer Keys and String Data ===\n");
    
    Tree23 *tree = create_tree(compare_int, print_int, print_string, free_dynamic, free_dynamic);
    
    // Insert some test data
    char *test_data[] = {"Alice", "Bob", "Charlie", "David", "Eve", "Frank", "Grace", "Henry"};
    int num_test = sizeof(test_data) / sizeof(test_data[0]);
    
    printf("Inserting %d elements:\n", num_test);
    for (int i = 0; i < num_test; i++) {
        int *key = malloc(sizeof(int));
        *key = (i + 1) * 10; // 10, 20, 30, ...
        char *data = malloc(strlen(test_data[i]) + 1);
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
        int *key = malloc(sizeof(int));
        int *data = malloc(sizeof(int));
        
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
        int *key = malloc(sizeof(int));
        int *data = malloc(sizeof(int));
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