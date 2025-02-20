#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

typedef struct Camera {
    vec3 pos, dir, up, right;
    float yaw, pitch;
    float fov;
    mat4 view, projection;
} Camera;

void camera_init(vec3 pos, vec3 dir, float fov);
void camera_update(float dt);
Camera* camera_get(void);

void camera_compute_view_and_projection(void);
void camera_set_shader_projection_and_view(Shader);

#endif
