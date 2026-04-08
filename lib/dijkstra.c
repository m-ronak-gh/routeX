#include "pathfind.h"
#include "heap.h"
#include <string.h>

PathResult dijkstra(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count){
    PathResult result;
    memset(&result, 0, sizeof(result));
    if(start_node < 0 || end_node < 0) return result;

    static double g_cost[MAX_NODES];
    static int parent[MAX_NODES];
    static minHeap heap;
    static char settled[MAX_NODES];

    for (int i=0; i < g->n_nodes; i++) {
        g_cost[i] = INF;
        parent[i] = -1;
        settled[i] = 0;
    }
    heap_init(&heap);
    if(visited_count) *visited_count = 0;

    g_cost[start_node] = 0.0;
    heap_push(&heap, (uint32_t)start_node, 0.0);

    while(!heap_empty(&heap)){
        heapItem current = heap_pop(&heap);
        int u = (int)current.node_id;
        if(settled[u]) continue;
        settled[u] = 1;
        result.nodes_visited++;
        if(visited_nodes && visited_count )
            visited_nodes[(*visited_count)++] = u;
        if(u == end_node) break;

        uint32_t edge_id = g->head[u];
        while(edge_id != NO_EDGE){
            Edge* e = &g->edges[edge_id];
            int v = (int)e->to;
            double new_g_cost = g_cost[u] + e->weight;
            if(new_g_cost < g_cost[v]){
                g_cost[v] = new_g_cost;
                parent[v] = u;
                heap_push(&heap, (uint32_t)v, new_g_cost);
            }
            edge_id = e->next;
        }
    }

    if(parent[end_node] != -1 || start_node == end_node){
        int node = end_node;
        while (node != -1) {
            result.path[result.path_length++] = node;
            node = parent[node];
        }
        for(int i=0; i < result.path_length / 2; i++){
            int temp = result.path[i];
            result.path[i] = result.path[result.path_length - 1 - i];
            result.path[result.path_length - 1 - i] = temp;
        }
        result.total_distance = g_cost[end_node];
        result.found = 1;
    }
    return result;
}