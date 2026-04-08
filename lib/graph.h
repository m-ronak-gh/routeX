#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

// limits
#define MAX_NODES   5000
#define MAX_EDGES   10000
#define NO_EDGE     UINT32_MAX //end of adjacency list
#define INF         1e18       //a very large number 1.0 * 10^18 (representing infinity)

// A Node
typedef struct Node {
    double lat, lon; //geographical coordinates
    int sx, sy;      //screen coordinates
} Node;

// An Edge
typedef struct Edge {
    uint32_t to;     //destination node index
    double weight;   //length of the edge (distance between nodes)
    uint32_t next;   //index of the next edge in the adjacency list
} Edge;

// Graph structure
typedef struct Graph {
    Node nodes[MAX_NODES]; //array of nodes
    Edge edges[MAX_EDGES]; //array of edges
    uint32_t head[MAX_NODES]; //head of the adjacency list for each node
    int n_nodes; //number of nodes in the graph
    int n_edges; //number of edges in the graph
    //bounding box for the graph
    double min_lat, max_lat;
    double min_lon, max_lon;
} Graph;

typedef struct PathResult{
    int path[MAX_NODES]; //array to store the path (sequence of node indices)
    int path_length; //length of the path (number of nodes in the path)
    int nodes_visited; //number of nodes visited during the search (for performance analysis)
    double total_distance; //total distance of the path (sum of edge weights)
    int found; //flag to indicate if a path was found (1 for success, 0 for failure)
} PathResult;

// Function prototypes
//// load graph data from a file 
int graph_load(Graph* g, const char* filename);

//// project geographical coordinates to screen coordinates
void graph_project(
    Graph* g,
    int screen_width, int screen_height,
    double zoom,
    double center_lat, double center_lon,
    double pan_x, double pan_y
);

//// find the nearest node to a given point (px, py) in screen coordinates
int graph_nearest_node(const Graph* g, int px, int py);

//// print summary of the graph (number of nodes, edges, bounding box)
void graph_print_summary(const Graph* g);

#endif