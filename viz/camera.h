#ifndef CAMERA_H
#define CAMERA_H

typedef struct Camera {
    double center_lat;
    double center_lon;
    double zoom;
    double pan_x;
    double pan_y;
    int screen_width;
    int screen_height;
} Camera;

void camera_fit(Camera *cam, int screen_width, int screen_height, double min_lat, double max_lat, double min_lon, double max_lon);

void world_to_screen(const Camera *cam, double lat, double lon, int *sx, int *sy);

void screen_to_world(const Camera *cam, int sx, int sy, double *lat, double *lon);

void camera_pan(Camera *cam, double dx, double dy);
void camera_zoom(Camera *cam, double factor, int mouse_x, int mouse_y);

#endif