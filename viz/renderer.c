#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int renderer_init(RendererCtx *ctx, int width, int height) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    ctx->window = SDL_CreateWindow("Pathfinding Visualization", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!ctx->window) {
        SDL_Quit();
        return 0;
    }
    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ctx->renderer) {
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        return 0;
    }
    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);

    SDL_Color vis_cols[] = {COL_ASTAR, COL_DIJIKSTRA,COL_GREEDY, COL_BFS, COL_DFS};
    SDL_Color path_cols[] = {COL_PATH_ASTAR, COL_PATH_DIJIKSTRA, COL_PATH_GREEDY, COL_PATH_BFS, COL_PATH_DFS};

    for (int i = 0; i < NUM_ALGOS; i++) {
        ctx->algos[i].log = calloc(MAX_NODES, sizeof(int));
        ctx->algos[i].count = 0;
        ctx->algos[i].frame = 0;
        ctx->algos[i].enabled = 1;
        ctx->algos[i].vis_col = vis_cols[i];
        ctx->algos[i].path_col = path_cols[i];
        ctx->algos[i].result.found = 0;
    }
    ctx->anim_speed = 25;
    ctx->running = 0;
    ctx->done = 0;
    ctx->start_node = -1;
    ctx->end_node = -1;
    return 1;
}

void renderer_destroy(RendererCtx *ctx) {
    for (int i = 0; i < NUM_ALGOS; i++)
        free(ctx->algos[i].log);
    if (ctx->renderer) SDL_DestroyRenderer(ctx->renderer);
    if (ctx->window) SDL_DestroyWindow(ctx->window);
    SDL_Quit();
}

void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = (int)sqrt((double)(radius*radius - dy*dy));
        SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

static void draw_path_offset(SDL_Renderer *renderer, const Graph *graph, const int *path, int path_length, SDL_Color col, int offset) {
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    for (int i = 0; i < path_length - 1; i++) {
        int x1 = graph->nodes[path[i]].sx;
        int y1 = graph->nodes[path[i]].sy;
        int x2 = graph->nodes[path[i + 1]].sx;
        int y2 = graph->nodes[path[i + 1]].sy;
        double dx = x2 - x1, dy = y2 - y1;
        double len = sqrt(dx*dx + dy*dy) + 0.001; // avoid division by zero
        int ox = (int)(-dy / len * offset);
        int oy = (int)(dx / len * offset);
        SDL_RenderDrawLine(renderer, x1+ox, y1+oy, x2+ox, y2+oy);
    }
}

void renderer_update(RendererCtx *ctx) {
    if(!ctx->running) return;
    int all_done = 1;
    for (int i = 0; i < NUM_ALGOS; i++) {
        AlgoState *algo = &ctx->algos[i];
        if (!algo->enabled) continue;
        if (algo->frame < algo->count) {
            algo->frame += ctx->anim_speed;
            if (algo->frame > algo->count) algo->frame = algo->count;
            all_done = 0;
        }
    }
    if (all_done){ 
        ctx->running = 0;
        ctx->done = 1; 
    }
}

void renderer_draw(RendererCtx *ctx, const Graph *graph) {
    SDL_Renderer *renderer = ctx->renderer;

    SDL_SetRenderDrawColor(renderer, 22, 22, 34, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 60, 60, 72, 255);
    for (int u = 0; u < graph->n_nodes; u++) {
        for (uint32_t e = graph->head[u]; e != NO_EDGE; e = graph->edges[e].next) {
            int v = (int)graph->edges[e].to;
            SDL_RenderDrawLine(renderer, graph->nodes[u].sx, graph->nodes[u].sy, graph->nodes[v].sx, graph->nodes[v].sy);
        }
    }

    int offsets[] = {-4, -2, 0, 2, 4};
    for (int i = 0; i < NUM_ALGOS; i++) {
        AlgoState *algo = &ctx->algos[i];
        if (!algo->enabled) continue;
        SDL_SetRenderDrawColor(renderer, algo->vis_col.r, algo->vis_col.g, algo->vis_col.b, algo->vis_col.a);
        for (int j = 0; j < algo->frame; j++) {
            int node_id = algo->log[j];
            SDL_Rect dot = {graph->nodes[node_id].sx - 2, graph->nodes[node_id].sy - 2, 5, 5};
            SDL_RenderFillRect(renderer, &dot);
        }
    }

    if(ctx->done) {
        for (int i = 0; i < NUM_ALGOS; i++) {
            AlgoState *algo = &ctx->algos[i];
            if (!algo->enabled || !algo->result.found) continue;
            draw_path_offset(renderer, graph, algo->result.path, algo->result.path_length, algo->path_col, offsets[i]);
        }
    }

    if (ctx->start_node >= 0) {
        SDL_SetRenderDrawColor(renderer, 226, 74, 74, 255);
        draw_circle(renderer, graph->nodes[ctx->start_node].sx, graph->nodes[ctx->start_node].sy, 7);
    }
    if (ctx->end_node >= 0) {
        SDL_SetRenderDrawColor(renderer, 29, 158, 117, 255);
        draw_circle(renderer, graph->nodes[ctx->end_node].sx, graph->nodes[ctx->end_node].sy, 7);
    }

    SDL_RenderPresent(renderer);
}