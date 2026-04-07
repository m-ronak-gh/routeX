#include "pathfind.h"
#include <string.h>

PathResult bfs(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count){
    PathResult result;
    memset(&result, 0, sizeof(result));
    if(start_node < 0 || end_node < 0) return result;

    static int parent[MAX_NODES];
    static char settled[MAX_NODES];
    static int queue[MAX_NODES]; // simple circular queue for BFS
    int head = 0, tail = 0;

    for (int i=0; i < g->n_nodes; i++) {
        parent[i] = -1;
        settled[i] = 0;
    }
    if(visited_count) *visited_count = 0;

    settled[start_node] = 1;
    queue[tail++] = start_node;

    while(head != tail){
        int u = queue[head++];
        result.nodes_visited++;
        
        if(visited_nodes && visited_count )
            visited_nodes[(*visited_count)++] = u;
        if(u == end_node) break;

        uint32_t edge_id = g->head[u];
        while(edge_id != NO_EDGE){
            Edge* e = &g->edges[edge_id];
            int v = (int)e->to;
            if(!settled[v]){
                settled[v] = 1;
                parent[v] = u;
                queue[tail++] = v;
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
        result.found = 1;
    }
    return result;
}