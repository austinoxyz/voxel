#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

typedef struct Camera {
    vec3 pos, dir, up, right;
    float yaw, pitch;
    float fov;
    mat4 view, projection;
} Camera;

void camera_init(Camera *camera, vec3 pos, vec3 dir, float fov);
void camera_update(Camera *camera, float dt);
void camera_compute_view_and_projection(Camera *camera);

#endif
