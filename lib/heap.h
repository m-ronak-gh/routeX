#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

#define HEAP_MAX 5001

typedef struct {
    double priority;
    uint32_t node_id;
} heapItem;

typedef struct {
    heapItem data[HEAP_MAX];
    int pos[HEAP_MAX];
    int size;
} minHeap;

void heap_init(minHeap *h);
void heap_push(minHeap *h, double priority, uint32_t node_id);
heapItem heap_pop(minHeap *h);
void heap_decrease_key(minHeap *h, double new_priority, uint32_t node_id);
int heap_contains(const minHeap *h, uint32_t node_id);
int heap_empty(const minHeap *h);

#endif