#include "camera.h"
#include "window.h"
#include "shader.h"
#include "input.h"

void camera_init(Camera *camera, vec3 pos, vec3 dir, float fov)
{
    assert(camera);

    glm_vec3_copy((vec3){ 0, 1, 0 }, camera->up);
    glm_vec3_copy(pos, camera->pos);
    glm_mat4_identity(camera->view);
    glm_mat4_identity(camera->projection);

    camera->yaw = glm_rad(180);
    camera->pitch = 0;
    camera->fov = fov;

    camera_compute_view_and_projection(camera);
}

void camera_update(Camera *camera, float dt)
{
    const double centerx = window_get()->size.x / 2.0f;
    const double centery = window_get()->size.x / 2.0f;

    double cursorx, cursory;
    glfwGetCursorPos(window_get_handle(), &cursorx, &cursory);
    glfwSetCursorPos(window_get_handle(), (int) centerx, (int) centery); 

    static const float mousespeed = 0.0002f;

    const float yaw_off   = (float) mousespeed * dt * (centerx - cursorx);
    const float pitch_off = (float) mousespeed * dt * (centery - cursory);

    camera->yaw += yaw_off;
    camera->pitch = clamp(camera->pitch + pitch_off, glm_rad(-89), glm_rad(89));

    float movespeed = 0.003f; // 3 units/second
    if (keymodifier_is_down(KEYMODIFIER_L_SHIFT)) {
        movespeed *= 2.0f;
    }

    vec3 change;
    if (key_is_down('W')) {
        glm_vec3_scale(camera->dir, movespeed * dt, change);
        glm_vec3_add(camera->pos, change, camera->pos);
    }
    if (key_is_down('S')) {
        glm_vec3_scale(camera->dir, -(movespeed * dt), change);
        glm_vec3_add(camera->pos, change, camera->pos);
    }
    if (key_is_down('A')) {
        glm_vec3_scale(camera->right, movespeed * dt, change);
        glm_vec3_add(camera->pos, change, camera->pos);
    }
    if (key_is_down('D')) {
        glm_vec3_scale(camera->right, -(movespeed * dt), change);
        glm_vec3_add(camera->pos, change, camera->pos);
    }
    if (key_is_down('Q')) {
        glm_vec3_scale(camera->up, -(movespeed * dt), change);
        glm_vec3_add(camera->pos, change, camera->pos);
    }
    if (key_is_down('E')) {
        glm_vec3_scale(camera->up, movespeed * dt, change);
        glm_vec3_add(camera->pos, change, camera->pos);
    }

    camera_compute_view_and_projection(camera);
}

void camera_compute_view_and_projection(Camera *camera)
{
    glm_vec3_copy((vec3){
        cos(camera->pitch) * sin(camera->yaw),
        sin(camera->pitch),
        cos(camera->pitch) * cos(camera->yaw)
    }, camera->dir);
    glm_vec3_normalize(camera->dir);

    glm_vec3_cross((vec3){ 0, 1.0f, 0 }, camera->dir, camera->right);
    glm_vec3_normalize(camera->right);

    glm_vec3_cross(camera->dir, camera->right, camera->up);
    glm_vec3_normalize(camera->up);

    const float znear = 0.1f;
    const float zfar = 1000.0f;
    glm_look(camera->pos, camera->dir, camera->up, camera->view);
    glm_perspective(camera->fov, window_get()->aspect, znear, zfar, camera->projection);
}
