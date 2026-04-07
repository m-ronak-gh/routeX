#ifndef PATHFIND_H
#define PATHFIND_H

#include "graph.h"

PathResult astar(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count);
PathResult dijkstra(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count);
PathResult greedy(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count);
PathResult bfs(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count);
PathResult dfs(Graph* g, int start_node, int end_node, int *visited_nodes, int *visited_count);

// haversine distance function to calculate the distance between two points given their latitude and longitude
//// used by the A* and Greedy algorithms as the heuristic function
double haversine(double lat1, double lon1, double lat2, double lon2);

#endif