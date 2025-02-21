#include "camera.h"
#include "window.h"
#include "shader.h"
#include "input.h"

static Camera s_camera;
static bool s_camera_initialized = false;

Camera* camera_get(void)
{
    assert(s_camera_initialized);
    return &s_camera;
}

void camera_init(vec3 pos, vec3 dir, float fov)
{
    assert(!s_camera_initialized);
    glm_vec3_copy((vec3){ 0, 1, 0 }, s_camera.up);
    glm_vec3_copy(pos, s_camera.pos);
    glm_mat4_identity(s_camera.view);
    glm_mat4_identity(s_camera.projection);
    s_camera.yaw = glm_rad(180);
    s_camera.pitch = 0;
    s_camera.fov = fov;
    camera_compute_view_and_projection();
    s_camera_initialized = true;
}

void camera_update(float dt)
{
    const double centerx = window_get()->size.x / 2.0f;
    const double centery = window_get()->size.x / 2.0f;

    double cursorx, cursory;
    glfwGetCursorPos(window_get_handle(), &cursorx, &cursory);
    glfwSetCursorPos(window_get_handle(), (int) centerx, (int) centery); 

    static const float mousespeed = 0.0002f;

    const float yaw_off   = (float) mousespeed * dt * (centerx - cursorx);
    const float pitch_off = (float) mousespeed * dt * (centery - cursory);

    s_camera.yaw += yaw_off;
    s_camera.pitch = clamp(s_camera.pitch + pitch_off, glm_rad(-89), glm_rad(89));

    float movespeed = 0.003f; // 3 units/second
    if (keymodifier_is_down(KEYMODIFIER_L_SHIFT)) {
        movespeed *= 2.0f;
    }

    vec3 change;
    if (key_is_down('W')) {
        glm_vec3_scale(s_camera.dir, movespeed * dt, change);
        glm_vec3_add(s_camera.pos, change, s_camera.pos);
    }
    if (key_is_down('S')) {
        glm_vec3_scale(s_camera.dir, -(movespeed * dt), change);
        glm_vec3_add(s_camera.pos, change, s_camera.pos);
    }
    if (key_is_down('A')) {
        glm_vec3_scale(s_camera.right, movespeed * dt, change);
        glm_vec3_add(s_camera.pos, change, s_camera.pos);
    }
    if (key_is_down('D')) {
        glm_vec3_scale(s_camera.right, -(movespeed * dt), change);
        glm_vec3_add(s_camera.pos, change, s_camera.pos);
    }
    if (key_is_down('Q')) {
        glm_vec3_scale(s_camera.up, -(movespeed * dt), change);
        glm_vec3_add(s_camera.pos, change, s_camera.pos);
    }
    if (key_is_down('E')) {
        glm_vec3_scale(s_camera.up, movespeed * dt, change);
        glm_vec3_add(s_camera.pos, change, s_camera.pos);
    }

    camera_compute_view_and_projection();
}

void camera_compute_view_and_projection(void)
{
    glm_vec3_copy((vec3){
        cos(s_camera.pitch) * sin(s_camera.yaw),
        sin(s_camera.pitch),
        cos(s_camera.pitch) * cos(s_camera.yaw)
    }, s_camera.dir);
    glm_vec3_normalize(s_camera.dir);

    glm_vec3_cross((vec3){ 0, 1.0f, 0 }, s_camera.dir, s_camera.right);
    glm_vec3_normalize(s_camera.right);

    glm_vec3_cross(s_camera.dir, s_camera.right, s_camera.up);
    glm_vec3_normalize(s_camera.up);

    glm_look(s_camera.pos, s_camera.dir, s_camera.up, s_camera.view);
    glm_perspective(s_camera.fov, window_get()->aspect, 0.1f, 100.0f, s_camera.projection);
}

mat4* camera_get_view_mat4(void)
{
    assert(s_camera_initialized);

    return &s_camera.view;
}

mat4* camera_get_projection_mat4(void)
{
    assert(s_camera_initialized);

    return &s_camera.projection;
}

void camera_set_shader_projection_and_view(Shader shader)
{
    shader_set_uniform_mat4(shader, "projection", s_camera.projection);
    shader_set_uniform_mat4(shader, "view", s_camera.view);
}
