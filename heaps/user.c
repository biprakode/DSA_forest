#include "header.h"
#include <time.h>

#define MAX 100

void print_int(void *data) {
        printf("%d", *(int *)data);
}

void print_double(void *data) {
        printf("%0.2lf", *(double *)data);
}

int main() {
    heap *max_int_h = build_heap(20, "max");
    
    printf("Testing Max-Heap with random ints:\n");
    srand(time(0)); 
    for (int i = 0; i < MAX; i++) {
        int *val = (int *)malloc(sizeof(int));
        *val = rand() % 100;
        long int key = *val;
        heap_insert(&max_int_h, val, key);
    }
    
    print_queue(max_int_h, print_int);
    printf("Size: %d\n", get_size(max_int_h));
    printf("Peek: %d\n", *(int *)get_peek(max_int_h));
    
    elem *ext1 = extract_peek(&max_int_h);
    printf("Extracted: key=%ld, value=", ext1->key);
    print_int(ext1->data);
    printf("\n");
    free(ext1->data);
    free(ext1); 
    
    print_queue(max_int_h, print_int);
    
    elem **sorted_int = get_heap_sort(&max_int_h);
    printf("Heap Sorted (max, descending): ");
    for (int i = 0; i < MAX-1; i++) {
        printf("Key=%ld, ", sorted_int[i]->key);
        print_int(sorted_int[i]->data);
        printf(" ");
        free(sorted_int[i]->data);
        free(sorted_int[i]);
    }
    printf("\n");
    free(sorted_int);
    
    free_heap(max_int_h);

    heap *max_double_h = build_heap(15, "min");
    
    printf("Testing Max-Heap with random doubles:\n");
    srand(time(0)); 
    for (int i = 0; i < MAX; i++) {
        double *val = (double *)malloc(sizeof(double));
        *val = (rand() % 10000) / 100.0;
        long int key = (long int)(*val * 100);
        heap_insert(&max_double_h, val, key);
    }
    
    print_queue(max_double_h, print_double);
    printf("Size: %d\n", get_size(max_double_h));
    printf("Peek: %0.2lf\n", *(double *)get_peek(max_double_h));
    
    elem *ext2 = extract_peek(&max_double_h);
    printf("Extracted: key=%ld", ext1->key);
    print_double(ext2->data);
    printf("\n");
    free(ext2->data);
    free(ext2); 
    
    print_queue(max_double_h, print_double);
    
    elem **sorted_double = get_heap_sort(&max_double_h);
    printf("Heap Sorted (min, ascending): ");
    for (int i = 0; i < MAX-1; i++) {
        printf("Key=%ld, ", sorted_double[i]->key);
        print_double(sorted_double[i]->data);
        printf(" ");
        printf("\n");
        free(sorted_double[i]->data);
        free(sorted_double[i]);
    }
    printf("\n");
    free(sorted_double);
    
    free_heap(max_double_h);
}