#include "header.h"

heap *build_heap(int capacity , const char *type) {
    heap *heap_obj = (heap *)malloc(sizeof(heap));
    if (!heap_obj) {
        perror("Failed to allocate heap structure");
        return NULL;
    }
    heap_obj->capacity = capacity > 0 ? capacity : 32;
    heap_obj->data = (elem **)malloc(sizeof(elem *) * heap_obj->capacity);
    if (!heap_obj->data) {
        perror("Failed to allocate heap array");
        free(heap_obj);
        return NULL;
    }
    memset(heap_obj->data, 0, sizeof(elem *) * heap_obj->capacity);
    heap_obj->size = 0;
    if (strcmp(type , "min") == 0) heap_obj->is_max = false;
    else if (strcmp(type , "max") == 0) heap_obj->is_max = true;
    else {
        printf("Warning: Unknown heap type '%s'. Defaulting to Max-Heap.\n", type);
        heap_obj->is_max = true;
    }
    return heap_obj;
}

void resize(heap **heap_obj , size_t new_cap) {
    elem **new_data = (elem **)realloc((*heap_obj)->data, sizeof(elem *) * new_cap);
    if (new_data == NULL) {
        perror("Failed to resize heap array");
        return; 
    }
    (*heap_obj)->data = new_data;
    (*heap_obj)->capacity = new_cap;
}

bool is_empty(heap *heap_obj) {
    return heap_obj->size == 0;
}

int get_size(heap *heap_obj) {
    return heap_obj->size;
}

void *get_peek(heap *heap_obj) {
    if (is_empty(heap_obj)) {
        printf("Heap empty\n");
        return NULL;
    } 
    return heap_obj->data[0]->data;
}

void swap(heap *heap_obj , int index1 , int index2) {
    elem *temp = heap_obj->data[index1];
    heap_obj->data[index1] = heap_obj->data[index2];
    heap_obj->data[index2] = temp;
}

int compare(heap *heap_obj , int index1 , int index2) {
    long int key1 = heap_obj->data[index1]->key;
    long int key2 = heap_obj->data[index2]->key;
    if(heap_obj->is_max) {
        if (key1 > key2) return 1;
        if (key1 < key2) return -1;
        return 0;
    } else {
        if (key1 < key2) return 1;
        if (key1 > key2) return -1;
        return 0;
    }
}

void heapify_down(heap **heap_obj , int index) {
    heap *h = *heap_obj;
    int highest = index;
    int left_ch = LEFT(index);
    int right_ch = RIGHT(index);
    int n = h->size;
    if (left_ch < n && compare(h, left_ch, highest) > 0) {
        highest = left_ch;
    }
    if (right_ch < n && compare(h, right_ch, highest) > 0) {
        highest = right_ch;
    }
    if (highest != index) {
        swap(h, highest, index);
        heapify_down(&h, highest);
    }
} 

void build_my_heap(heap **heap_obj) {
    heap *h = *heap_obj;
    for(int i = h->size/2 - 1 ; i>=0 ; i--) {
        heapify_down(heap_obj , i );
    }
}

void increase_key(heap *heap_obj , int index) {
    int parent = PARENT(index);
    while (index > 0 && compare(heap_obj, index, parent) > 0) {
        swap(heap_obj, index, parent);
        index = parent;
        parent = PARENT(index);
    }
}

void heap_insert(heap **heap_obj , void *user_elem , long int key) {
    heap *h = *heap_obj;
    if(h->size >= h->capacity) {
        h->capacity <<= 1;
        resize(heap_obj , h->capacity);
    }
    elem *new_elem = (elem *)malloc(sizeof(elem));
    if (!new_elem) {
        perror("Failed to allocate new element");
        return;
    }
    new_elem->data = user_elem;
    new_elem->key = key;

    int idx = h->size;
    h->data[idx] = new_elem;
    h->size++;
    increase_key(h , idx);
}

elem *extract_peek(heap **heap_obj) {
    heap *h = *heap_obj;
    if(is_empty(h)) {
        printf("Heap empty\n");
        return NULL;
    }
    elem *top = h->data[0];
    h->data[0] = h->data[h->size-1];
    h->size--;
    heapify_down(&h , 0);
    return top;
}

elem **get_heap_sort(heap **heap_obj) {
    heap *h = *heap_obj;
    int original_size = h->size;
    elem **sorted = (elem **)malloc(sizeof(elem *) * original_size);
    for (int i = 0; i < original_size; i++) {
        sorted[i] = extract_peek(heap_obj);
    }
    return sorted;
}

void heap_sort(heap **heap_obj) {
    heap *h = *heap_obj;
    int original_size = h->size;
    for (int i = h->size - 1; i >= 1; i--) {
        swap(h , 0 , i);
        h->size--;
        heapify_down(&h , 0);
    }
    h->size = original_size;
}

void print_queue(heap *heap_obj, void (*print_elem)(void *)) {
    if (is_empty(heap_obj)) {
        printf("Heap empty\n");
        return;
    }
    printf("Heap (%s-heap, size=%d): \n", heap_obj->is_max ? "max" : "min", heap_obj->size);
    for (int i = 0; i < heap_obj->size; i++) {
        printf("Index %d: Key=%ld, Data=", i, heap_obj->data[i]->key);
        if (print_elem) {
            print_elem(heap_obj->data[i]->data);
        } else {
            printf("(no print func)");
        }
        printf("\n");
    }
}

void build_heap_from_array(heap *heap_obj , elem **elements , int num_elements) {
    if (num_elements > heap_obj->capacity) {
        resize(&heap_obj, num_elements * 2);
    }
    memcpy(heap_obj->data, elements, sizeof(elem *) * num_elements);
    heap_obj->size = num_elements;
    build_my_heap(&heap_obj);
}

void free_heap(heap *heap_obj) {
    for (int i = 0; i < heap_obj->size; i++) {
        free(heap_obj->data[i]);
    }
    free(heap_obj->data);
    free(heap_obj);
}