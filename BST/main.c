#include "header.h"
#include <time.h>

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
