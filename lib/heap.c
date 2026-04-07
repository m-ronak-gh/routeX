#include "heap.h"
#include <string.h>

// helpers
static void swap(minHeap *h, int i, int j) {
    heapItem temp = h->data[i];
    h->data[i] = h->data[j];
    h->data[j] = temp;
    h->pos[h->data[i].node_id] = i;
    h->pos[h->data[j].node_id] = j;
}

static void shift_up(minHeap *h, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (h->data[index].priority < h->data[parent].priority) {
            swap(h, index, parent);
            index = parent;
        } else break;
    }
}

static void shift_down(minHeap *h, int index) {
    int n = h->size;
    for (;;) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < n && h->data[left].priority < h->data[smallest].priority) {
            smallest = left;
        }
        if (right < n && h->data[right].priority < h->data[smallest].priority) {
            smallest = right;
        }
        if (smallest != index) {
            swap(h, index, smallest);
            index = smallest;
        } else break;
    }
}

// public API
void heap_init(minHeap *h) {
    h->size = 0;
    memset(h->pos, -1, sizeof(h->pos));
}

void heap_push(minHeap *h, uint32_t node_id, double priority) {
    int index = h->size++;
    h->data[index].node_id = node_id;
    h->data[index].priority = priority;
    h->pos[node_id] = index;
    shift_up(h, index);
}

heapItem heap_pop(minHeap *h) {
    heapItem top = h->data[0];
    h->pos[top.node_id] = -1;
    h->size--;
    if (h->size > 0) {
        h->pos[h->data[0].node_id] = 0;
        shift_down(h, 0);
    }
    return top;
}

void heap_decrease_key(minHeap *h, uint32_t node_id, double new_priority) {
    int index = h->pos[node_id];
    h->data[index].priority = new_priority;
    shift_up(h, index);
}

int heap_contains(const minHeap *h, uint32_t node_id) {
    return h->pos[node_id] >= 0;
}

int heap_empty(const minHeap *h) {
    return h->size == 0;
}