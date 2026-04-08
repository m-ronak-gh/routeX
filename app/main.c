#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/graph.h"
#include "../lib/pathfind.h"
#include "../viz/renderer.h"
#include "../viz/camera.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 700
#define PAN_STEP 20.0

static Graph g;

static void run_algorithm(RendererCtx *ctx){
    if(ctx->start_node < 0 || ctx->end_node < 0 ){
        fprintf(stderr, "Invalid start or end node\n");
        return;
    }
    int s = ctx->start_node;
    int e = ctx->end_node;

    for(int i = 0; i < NUM_ALGOS; i++){
        ctx->algos[i].count = 0;
        ctx->algos[i].frame = 0;
        ctx->algos[i].result.found = 0;
    }
    ctx->done = 0;
    ctx->running = 1;

    printf("Running A*...\n");
    ctx->algos[0].result = astar(&g, s, e, ctx->algos[0].log, &ctx->algos[0].count);
    printf("%d nodes visited, path length: %d, total distance: %f\n", ctx->algos[0].count, ctx->algos[0].result.path_length, ctx->algos[0].result.total_distance);

    printf("Running Dijkstra...\n");
    ctx->algos[1].result = dijkstra(&g, s, e, ctx->algos[1].log, &ctx->algos[1].count);
    printf("%d nodes visited, path length: %d, total distance: %f\n", ctx->algos[1].count, ctx->algos[1].result.path_length, ctx->algos[1].result.total_distance);

    printf("Running Greedy...\n");
    ctx->algos[2].result = greedy(&g, s, e, ctx->algos[2].log, &ctx->algos[2].count);
    printf("%d nodes visited, path length: %d, total distance: %f\n", ctx->algos[2].count, ctx->algos[2].result.path_length, ctx->algos[2].result.total_distance);

    printf("Running BFS...\n");
    ctx->algos[3].result = bfs(&g, s, e, ctx->algos[3].log, &ctx->algos[3].count);
    printf("%d nodes visited, path length: %d, total distance: %f\n", ctx->algos[3].count, ctx->algos[3].result.path_length, ctx->algos[3].result.total_distance);

    printf("Running DFS...\n");
    ctx->algos[4].result = dfs(&g, s, e, ctx->algos[4].log, &ctx->algos[4].count);
    printf("%d nodes visited, path length: %d, total distance: %f\n", ctx->algos[4].count, ctx->algos[4].result.path_length, ctx->algos[4].result.total_distance);
}

int main(int argc, char *argv[]){
    const char *map_file = (argc > 1) ? argv[1] : "data/cambridge.bin";
    if (!graph_load(&g, map_file)) {
        fprintf(stderr, "Failed to load graph from: %s\n", map_file);
        return EXIT_FAILURE;
    }
    graph_print_summary(&g);

    Camera cam;
    camera_fit(&cam, SCREEN_WIDTH, SCREEN_HEIGHT, g.min_lat, g.max_lat, g.min_lon, g.max_lon);

    graph_project(&g, SCREEN_WIDTH, SCREEN_HEIGHT,
                cam.zoom, cam.center_lat, cam.center_lon,
                cam.pan_x, cam.pan_y);

    RendererCtx ctx;
    if(!renderer_init(&ctx, SCREEN_WIDTH, SCREEN_HEIGHT)) return 1;

    int click_phase = 0;
    printf("\nControls:\n");
    printf("    Click       : set start(1st) / end(2nd) point\n");
    printf("    R           : run all 5 algorithms\n");
    printf("    C           : clear selection\n");
    printf("    Arrow keys  : pan camera\n");
    printf("    + / -       : zoom in / out\n");
    printf("    A D G B F   : toggle A* / Dijkstra / Greedy / BFS / DFS\n");
    printf("    []          : decrease / increase animation speed\n");
    printf("    Esc / Q     : exit / quit\n\n");

    int quit = 0;
    SDL_Event ev;
    while(!quit){
        while(SDL_PollEvent(&ev)){
            switch (ev.type){
                case SDL_QUIT:
                    quit = 1; break;
                
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym){
                        case SDLK_ESCAPE: case SDLK_q: quit = 1; break;

                        case SDLK_LEFT: camera_pan(&cam, -PAN_STEP, 0); break;
                        case SDLK_RIGHT: camera_pan(&cam, PAN_STEP, 0); break;
                        case SDLK_UP: camera_pan(&cam, 0, -PAN_STEP); break;
                        case SDLK_DOWN: camera_pan(&cam, 0, PAN_STEP); break;
                        case SDLK_PLUS: case SDLK_EQUALS:
                            camera_zoom(&cam, 1.2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2); break;
                        case SDLK_MINUS:
                            camera_zoom(&cam, 1.0/1.2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2); break;
                        
                        case SDLK_a:
                            ctx.algos[0].enabled ^= 1;
                            printf("A*      : %s\n", ctx.algos[0].enabled?"ON":"OFF"); break;
                        case SDLK_d:
                            ctx.algos[1].enabled ^= 1;
                            printf("Dijkstra: %s\n", ctx.algos[1].enabled?"ON":"OFF"); break;
                        case SDLK_g:
                            ctx.algos[2].enabled ^= 1;
                            printf("Greedy  : %s\n", ctx.algos[2].enabled?"ON":"OFF"); break;
                        case SDLK_b:
                            ctx.algos[3].enabled ^= 1;
                            printf("BFS     : %s\n", ctx.algos[3].enabled?"ON":"OFF"); break;
                        case SDLK_f:
                            ctx.algos[4].enabled ^= 1;
                            printf("DFS     : %s\n", ctx.algos[4].enabled?"ON":"OFF"); break;
                        

                        case SDLK_LEFTBRACKET:
                            if(ctx.anim_speed > 1)
                                ctx.anim_speed -= 5;
                            printf("Speed: %d\n", ctx.anim_speed); break;
                        case SDLK_RIGHTBRACKET:
                            ctx.anim_speed += 5;
                            printf("Speed: %d\n", ctx.anim_speed); break;

                        case SDLK_r:
                            run_algorithm(&ctx); break;
                        
                        case SDLK_c:
                            ctx.start_node = -1;
                            ctx.end_node = -1;
                            ctx.running = 0;
                            ctx.done = 0;
                            click_phase = 0;
                            for (int i=0; i<NUM_ALGOS; i++){
                                ctx.algos[i].count = 0;
                                ctx.algos[i].frame = 0;
                            }
                            printf("Cleared. Click to set start point.\n");
                            break;
                        }
                    graph_project(&g, SCREEN_WIDTH, SCREEN_HEIGHT,
                                cam.zoom, cam.center_lat, cam.center_lon,
                                cam.pan_x, cam.pan_y);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if(ev.button.button == SDL_BUTTON_LEFT){
                        int nearest = graph_nearest_node(&g, ev.button.x, ev.button.y);
                        if(click_phase == 0){
                            ctx.start_node = nearest;
                            click_phase = 1;
                            printf("Start set: node %d""(lat=%.4f lon=%.4f)\n", nearest, g.nodes[nearest].lat, g.nodes[nearest].lon);
                        } else{
                            ctx.end_node = nearest;
                            click_phase = 0;
                            printf("End set: node %d""(lat=%.4f lon=%.4f)\n""Press R to run.\n", nearest, g.nodes[nearest].lat, g.nodes[nearest].lon);
                        }
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    int mx, my;
                    SDL_GetMouseState(&mx, &my);
                    double factor = (ev.wheel.y > 0) ? 1.15 : 1.0/1.15;
                    camera_zoom(&cam, factor, mx, my);
                    graph_project(&g, SCREEN_WIDTH, SCREEN_HEIGHT,
                                cam.zoom, cam.center_lat, cam.center_lon,
                                cam.pan_x, cam.pan_y);
                    break;
            }
        }
        renderer_update(&ctx);
        renderer_draw(&ctx, &g);
        SDL_Delay(16);
    }
    renderer_destroy(&ctx);
    return 0;
}