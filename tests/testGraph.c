#include <stdio.h>
#include <assert.h>
#include "../lib/graph.h"

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "data/cambridge.bin";
    Graph g;

    int ok = graph_load(&g, filename);
    assert(ok && "Failed to load graph");
    assert(g.n_nodes > 0);
    assert(g.n_edges > 0);
    assert(g.min_lat < g.max_lat);
    assert(g.min_lon < g.max_lon);

    for(int i = 0; i < g.n_nodes; i++) {
        assert(g.nodes[i].lat >= g.min_lat - 0.01);
        assert(g.nodes[i].lat <= g.max_lat + 0.01);
        assert(g.nodes[i].lon >= g.min_lon - 0.01);
        assert(g.nodes[i].lon <= g.max_lon + 0.01);
    }

    graph_print_summary(&g);
    printf("All tests passed successfully.\n");
    return 0;
}