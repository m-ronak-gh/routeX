#include "graph.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int graph_load(Graph* g, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return 0;
    }

    // header
    uint32_t n_nodes, n_edges;
    if (fread(&n_nodes, sizeof(uint32_t), 1, f) != 1 ||
        fread(&n_edges, sizeof(uint32_t), 1, f) != 1) {
        fprintf(stderr, "Error reading header from file: %s\n", filename);
        fclose(f);
        return 0;
    }
    if (n_nodes > MAX_NODES || n_edges > MAX_EDGES) {
        fprintf(stderr, "File contains too many nodes or edges: %s\n", filename);
        fclose(f);
        return 0;
    }

    g->n_nodes = (int)n_nodes;
    g->n_edges = (int)n_edges;

    //initialize adjacency list heads to NO_EDGE
    for(int i=0; i<(g->n_nodes); i++)
        g->head[i] = NO_EDGE;
    
    //compute bounding box
    g->min_lat = 1e9; g->max_lat = -1e9;
    g->min_lon = 1e9; g->max_lon = -1e9;

    //read nodes
    for (int i = 0; i < (g->n_nodes); i++) {
        double lat, lon;
        if (fread(&lat, sizeof(double), 1, f) != 1 ||
            fread(&lon, sizeof(double), 1, f) != 1) {
            fprintf(stderr, "Error reading node data from file: %s\n", filename);
            fclose(f);
            return 0;
        }

        g->nodes[i].lat = lat;
        g->nodes[i].lon = lon;
        g->nodes[i].sx = 0;
        g->nodes[i].sy = 0;

        if (lat < g->min_lat) g->min_lat = lat;
        if (lat > g->max_lat) g->max_lat = lat;
        if (lon < g->min_lon) g->min_lon = lon;
        if (lon > g->max_lon) g->max_lon = lon;
    }

    //read edges
    for (int i=0; i< g->n_edges; i++){
        uint32_t from_id, to_id;
        double weight;
        if (fread(&from_id, sizeof(uint32_t), 1, f) != 1 ||
            fread(&to_id, sizeof(uint32_t), 1, f) != 1 ||
            fread(&weight, sizeof(double), 1, f) != 1) {
            fprintf(stderr, "Error reading edge data from file: %s\n", filename);
            fclose(f);
            return 0;
        }

        g->edges[i].to = to_id;
        g->edges[i].weight = weight;
        g->edges[i].next = g->head[from_id];
        g->head[from_id] = (uint32_t)i;
    }

    fclose(f);
    printf("Graph load: %d nodes, %d edges from %s\n", g->n_nodes, g->n_edges, filename);
    return 1;
}

void graph_project(Graph *g,
                   int screen_width, int screen_height,
                   double zoom,
                   double center_lat, double center_lon,
                   double pan_x, double pan_y) {
    for (int i = 0; i < g->n_nodes; i++) {
        double dx = (g->nodes[i].lon - center_lon) * zoom;
        double dy = (g->nodes[i].lat - center_lat) * zoom;
        g->nodes[i].sx = (int)(screen_width  * 0.5 + dx + pan_x);
        g->nodes[i].sy = (int)(screen_height * 0.5 - dy + pan_y);
    }
}

int graph_nearest_node(const Graph* g, int px, int py){
    int best = -1;
    double best_d = INF;
    for(int i=0; i< g->n_nodes; i++){
        double dx = g->nodes[i].sx - px;
        double dy = g->nodes[i].sy - py;
        double d = dx*dx + dy*dy;
        if(d< best_d){
            best_d = d;
            best = i;
        }
    }
    return best;
}

void graph_print_summary(const Graph* g){
    printf("Graph summary: %d nodes, %d edges\n", g->n_nodes, g->n_edges);
    printf("    Lat range: %.4f -- %.4f\n", g->min_lat, g->max_lat);
    printf("    Lon range: %.4f -- %.4f\n", g->min_lon, g->max_lon);
    int lim = g->n_nodes < 3 ? g->n_nodes : 3;
    for (int i=0; i<lim; i++)
        printf("[%d] lat=%.5f lon=%.5f\n", i, g->nodes[i].lat, g->nodes[i].lon);
}