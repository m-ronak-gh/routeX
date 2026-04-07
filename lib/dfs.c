#include "pathfind.h"
#include <string.h>

PathResult dfs(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count){
    PathResult result;
    memset(&result, 0, sizeof(result));
    if(start_node < 0 || end_node < 0) return result;

    static int parent[MAX_NODES];
    static char settled[MAX_NODES];
    static int stack[MAX_NODES]; // simple stack for DFS
    int top = 0;

    for (int i=0; i < g->n_nodes; i++) {
        parent[i] = -1;
        settled[i] = 0;
    }
    if(visited_count) *visited_count = 0;

    settled[start_node] = 1;
    stack[top++] = start_node;

    while(top > 0){
        int u = stack[--top];
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
            if(!settled[v]){
                parent[v] = u;
                stack[top++] = v;
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