#include "pathfind.h"
#include "heap.h"
#include <math.h>
#include <string.h>

#define DEG2RAD (3.14159265358979323846 / 180.0) //degrees to radians conversion
#define EARTH_RADIUS 6371000.0 //in meters

double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dlat = (lat2 - lat1) * DEG2RAD;
    double dlon = (lon2 - lon1) * DEG2RAD;
    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1 * DEG2RAD) * cos(lat2 * DEG2RAD) *
               sin(dlon / 2) * sin(dlon / 2);
    return EARTH_RADIUS * 2 * atan2(sqrt(a), sqrt(1 - a));
}

PathResult astar(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count) {
    // Implementation for A* algorithm
    PathResult result;
    memset(&result, 0, sizeof(result)); //initialize result
    if (start_node < 0 || end_node < 0) return result;

    static double g_cost[MAX_NODES]; //cost from start to node
    static int parent[MAX_NODES]; //to reconstruct path
    static minHeap heap;
    static char settled[MAX_NODES]; //to track settled nodes

    for (int i = 0; i < g->n_nodes; i++) {
        g_cost[i] = INF;
        parent[i] = -1;
        settled[i] = 0;
    }
    heap_init(&heap);
    if (visited_count) *visited_count = 0;
    g_cost[start_node] = 0.0;
    heap_push(&heap, (uint32_t)start_node, 0.0);

    while(!heap_empty(&heap)){
        heapItem current = heap_pop(&heap);
        int u = (int)current.node_id;
        if (settled[u]) continue; //already processed
        settled[u] = 1; //mark as settled
        result.nodes_visited++;
        if (visited_nodes && visited_count)
            visited_nodes[(*visited_count)++] = u; //track visited nodes
        if (u == end_node) break; //found the target

        uint32_t edge_id = g->head[u];
        while(edge_id != NO_EDGE){
            Edge *e = &g->edges[edge_id];
            int v = (int)e->to;

            double new_g_cost = g_cost[u] + e->weight;

            if(new_g_cost < g_cost[v]){
                g_cost[v] = new_g_cost;
                parent[v] = u;
                double h_cost = haversine(g->nodes[v].lat, g->nodes[v].lon,
                                          g->nodes[end_node].lat, g->nodes[end_node].lon);
                heap_push(&heap, (uint32_t)v, new_g_cost + h_cost);
            }
            edge_id = e->next; //move to next edge
        }
    }

    if(parent[end_node] != -1 || end_node == start_node){
        int node = end_node;
        while(node != -1){
            result.path[result.path_length++] = node; //store path in reverse
            node = parent[node];
        }
        //reverse path
        for(int i = 0; i < result.path_length / 2; i++){
            int temp = result.path[i];
            result.path[i] = result.path[result.path_length - 1 - i];
            result.path[result.path_length - 1 - i] = temp;
        }
        result.found = 1;
        result.total_distance = g_cost[end_node];
    }
    return result;
}