#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LEFT(i) (2 * (i) + 1)
#define RIGHT(i) (2 * (i) + 2)
#define PARENT(i) (((i) - 1) / 2)

typedef struct elem_struct {
    void *data;       
    long int key;     
} elem;

typedef struct heap_struct {
    int size;
    bool is_max; 
    int capacity;
    elem **data;
} heap;

void resize(heap **heap_obj , size_t new_cap);
int get_size(heap *heap_obj);
void *get_peek(heap *heap_obj);
void swap(heap *heap_obj , int index1 , int index2);
int compare(heap *heap_obj, int index1, int index2);
void increase_key(heap *heap_obj , int index);
heap *build_heap(int capacity , const char *type);
void build_my_heap(heap **heap_obj);
void build_heap_from_array(heap *heap_obj , elem **elements , int num_elements);  
void heapify_down(heap **heap_obj , int index);
void heap_insert(heap **heap_obj , void *user_elem , long int key);
elem *extract_peek(heap **heap_obj); 
elem **get_heap_sort(heap **heap_obj); 
void heap_sort(heap **heap_obj);
void print_queue(heap *heap_obj, void (*print_elem)(void *)); 
void free_heap(heap *heap_obj);
bool is_empty(heap *heap_obj);