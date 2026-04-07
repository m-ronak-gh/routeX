#include <stdio.h>
#include <assert.h>
#include "../lib/heap.h"

int main() {
    minHeap h;
    heap_init(&h);

    // Test insert out of order priorities
    heap_push(&h, 4, 40.0);
    heap_push(&h, 1, 10.0);
    heap_push(&h, 3, 30.0);
    heap_push(&h, 0, 5.0);
    heap_push(&h, 2, 20.0);

    //pop should return the smallest priority first
    assert(heap_pop(&h).node_id == 0); // 5.0
    assert(heap_pop(&h).node_id == 1); // 10.0
    assert(heap_pop(&h).node_id == 2); // 20.0
    assert(heap_pop(&h).node_id == 3); // 30.0
    assert(heap_pop(&h).node_id == 4); // 40.0
    assert(heap_empty(&h));

    //test decrease key
    heap_init(&h);
    heap_push(&h, 10, 100.0);
    heap_push(&h, 20, 50.0);
    heap_push(&h, 30, 80.0);
    // node 10 should be minimum
    heap_decrease_key(&h, 10, 10.0);
    assert(heap_pop(&h).node_id == 10); // 10.0
    assert(heap_pop(&h).node_id == 20); // 50.0

    printf("All tests passed!\n");
    return 0;
}