#include "player.h"
#include "window.h"
#include "input.h"

int player_init(Player *player, World *world)
{
    glm_vec3_zero(player->vel);
    glm_vec3_copy(player->pos, (vec3){ 0, 0, 3 });

    player->world = world;

    camera_init(&player->camera, player->pos, (vec3){ 0, 0, -1 }, 45.0f);

    return 0;
}

void player_deinit(Player *player)
{
    assert(player);
    UNUSED(player);
}

void player_handle_input(Player *player, float dt)
{
    const double centerx = window_get()->size.x / 2.0f;
    const double centery = window_get()->size.x / 2.0f;

    double cursorx, cursory;
    glfwGetCursorPos(window_get_handle(), &cursorx, &cursory);
    glfwSetCursorPos(window_get_handle(), (int) centerx, (int) centery); 

    static const float mousespeed = 0.0002f;

    const float yaw_off   = (float) mousespeed * dt * (centerx - cursorx);
    const float pitch_off = (float) mousespeed * dt * (centery - cursory);

    player->camera.yaw += yaw_off;
    player->camera.pitch = clamp(player->camera.pitch + pitch_off, glm_rad(-89), glm_rad(89));

    vec3 tmp;

    // FIXME: no worky

    if (key_pressed('W')) {
        glm_vec3_copy(tmp, player->camera.dir);
        glm_vec3_normalize(tmp);
        glm_vec3_add(player->vel, tmp, player->vel);
    } else if (key_released('W')) {
        glm_vec3_copy(tmp, player->camera.dir);
        glm_vec3_normalize(tmp);
        glm_vec3_sub(player->vel, tmp, player->vel);
    }

    if (key_pressed('S')) {
        glm_vec3_copy(tmp, player->camera.dir);
        glm_vec3_normalize(tmp);
        glm_vec3_negate(tmp);
        glm_vec3_add(player->vel, tmp, player->vel);
    } else if (key_released('S')) {
        glm_vec3_copy(tmp, player->camera.dir);
        glm_vec3_normalize(tmp);
        glm_vec3_negate(tmp);
        glm_vec3_sub(player->vel, tmp, player->vel);
    }

    if (key_pressed('A')) {
        glm_vec3_copy(tmp, player->camera.right);
        glm_vec3_normalize(tmp);
        glm_vec3_negate(tmp);
        glm_vec3_add(player->vel, tmp, player->vel);
    } else if (key_released('A')) {
        glm_vec3_copy(tmp, player->camera.right);
        glm_vec3_normalize(tmp);
        glm_vec3_negate(tmp);
        glm_vec3_sub(player->vel, tmp, player->vel);
    }

    if (key_pressed('D')) {
        glm_vec3_copy(tmp, player->camera.right);
        glm_vec3_normalize(tmp);
        glm_vec3_add(player->vel, tmp, player->vel);
    } else if (key_released('D')) {
        glm_vec3_copy(tmp, player->camera.right);
        glm_vec3_normalize(tmp);
        glm_vec3_sub(player->vel, tmp, player->vel);
    }
    
    // TODO: jumping
}

void player_update(Player *player, float dt)
{
    assert(player);

    static const float move_speed = 1.0f;
    vec3 move_off;
    glm_vec3_copy(move_off, player->vel);
    glm_vec3_scale(player->vel, move_speed * dt, player->vel);
    glm_vec3_add(player->pos, move_off, player->pos);

    glm_vec3_copy(player->camera.pos, player->pos);

    // FIXME: use camera_compute_view_and_projection after player movement input works
    camera_update(&player->camera, dt);
    /* camera_compute_view_and_projection(&player->camera); */
}

void player_render(Player *player, float dt)
{
    assert(player);

    UNUSED(player);
}
