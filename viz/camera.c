#include "camera.h"
#include <math.h>

void camera_fit(Camera *cam, int screen_width, int screen_height, double min_lat, double max_lat, double min_lon, double max_lon) {
    cam->screen_width = screen_width;
    cam->screen_height = screen_height;
    cam->center_lat = (min_lat + max_lat) * 0.5;
    cam->center_lon = (min_lon + max_lon) * 0.5;
    cam->pan_x = 0.0;
    cam->pan_y = 0.0;

    double span_lat = (max_lat - min_lat) * 1.1 + 1e-9;
    double span_lon = (max_lon - min_lon) * 1.1 + 1e-9;
    double zoom_lat = screen_height / span_lat;
    double zoom_lon = screen_width / span_lon;
    cam->zoom = fmin(zoom_lon, zoom_lat);
}

void world_to_screen(const Camera *cam, double lat, double lon, int *sx, int *sy) {
    double dx = (lon - cam->center_lon) * cam->zoom + cam->pan_x;
    double dy = (lat - cam->center_lat) * cam->zoom + cam->pan_y;
    *sx = (int)(cam->screen_width / 2 + dx);
    *sy = (int)(cam->screen_height / 2 - dy);
}

void screen_to_world(const Camera *cam, int sx, int sy, double *lat, double *lon) {
    double dx = sx - cam->screen_width / 2 - cam->pan_x;
    double dy = cam->screen_height / 2 - sy - cam->pan_y;
    *lon = cam->center_lon + dx / cam->zoom;
    *lat = cam->center_lat + dy / cam->zoom;
}

void camera_pan(Camera *cam, double dx, double dy) {
    cam->pan_x += dx;
    cam->pan_y += dy;
}

void camera_zoom(Camera *cam, double factor, int mouse_x, int mouse_y) {
    double pre_lat, pre_lon;
    screen_to_world(cam, mouse_x, mouse_y, &pre_lat, &pre_lon);

    cam->zoom *= factor;

    int post_sx, post_sy;
    world_to_screen(cam, pre_lat, pre_lon, &post_sx, &post_sy);

    cam->pan_x += (double)(mouse_x - post_sx);
    cam->pan_y += (double)(mouse_y - post_sy);
}