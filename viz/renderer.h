#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include "../lib/graph.h"
#include "../lib/pathfind.h"
#include "camera.h"

#define COL_ASTAR           {55, 138, 221, 180} //blue
#define COL_DIJIKSTRA       {99, 153, 34, 180} //green
#define COL_GREEDY          {186, 117, 23, 180} //amber
#define COL_BFS             {127, 119, 221, 180} //purple
#define COL_DFS             {216, 90, 48, 180} //coral
#define COL_PATH_ASTAR      {55, 138, 221, 255}
#define COL_PATH_DIJIKSTRA  {99, 153, 34, 255}
#define COL_PATH_GREEDY     {186, 117, 23, 255}
#define COL_PATH_BFS        {127, 119, 221, 255}
#define COL_PATH_DFS        {216, 90, 48, 255}
#define COL_STREET          {60, 60, 72, 255}
#define COL_MAPBG           {22, 22, 34, 255}
#define COL_START           {226, 74, 74, 255}
#define COL_END             {29, 158, 117, 255}

#define NUM_ALGOS 5

typedef struct AlgoState{
    int *log;
    int count;
    int frame;
    int enabled;
    SDL_Color vis_col;
    SDL_Color path_col;
    PathResult result;
} AlgoState;

typedef struct RendererCtx{
    SDL_Window *window;
    SDL_Renderer *renderer;
    AlgoState algos[NUM_ALGOS];
    int anim_speed;
    int running;
    int done;
    int start_node;
    int end_node;
} RendererCtx;

int renderer_init(RendererCtx *ctx, int width, int height);
void renderer_destroy(RendererCtx *ctx);

void renderer_draw(RendererCtx *ctx, const Graph *graph);
void renderer_update(RendererCtx *ctx);
void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius);

#endif