#include "player.h"
#include "window.h"
#include "input.h"
#include "world.h"

#include "playervertices.h"

void player_compute_camera_pos(Player *player, vec3 result);

void player_compute_bbox(Player *player);
void player_load_vertices(Player *player);

int player_init(Player *player, World *world)
{
    glm_vec3_zero(player->vel);
    glm_vec3_copy((vec3){ 3, 5, 3 }, player->pos);

    player->world    = world;
    player->width    = 0.7f;
    player->height   = 1.2f;
    player->grounded = false;

    vec3 campos;
    static const float fov = 45.0;
    player_compute_camera_pos(player, campos);
    camera_init(&player->camera, player->pos, campos, fov);

    player_compute_bbox(player);
    player_load_vertices(player);

    if (0 > shader_create(&player->shader, "src/glsl/player_vs.glsl", "src/glsl/player_fs.glsl")) {
        err("Failed to create shader for player model.");
        return -1;
    }

    glGenVertexArrays(1, &player->vao);
    glGenBuffers(1, &player->vbo);

    glUseProgram(player->shader);
    glBindVertexArray(player->vao);
    glBindBuffer(GL_ARRAY_BUFFER, player->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 36 * sizeof(PlayerModelVertex),
                 s_player_model_vertices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(PlayerModelVertex),
                          (GLvoid *) offsetof(PlayerModelVertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(PlayerModelVertex),
                          (GLvoid *) offsetof(PlayerModelVertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                          sizeof(PlayerModelVertex),
                          (GLvoid *) offsetof(PlayerModelVertex, color));
    glEnableVertexAttribArray(0);

    shader_set_uniform_vec3(player->shader, "light.color", &world->lightsource.color.raw[0]);
    shader_set_uniform_vec3(player->shader, "light.pos",   &world->lightsource.pos.raw[0]);
    shader_set_uniform_vec3(player->shader, "light.ambient",  (vec3){ 0.2, 0.2, 0.2 });
    shader_set_uniform_vec3(player->shader, "light.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(player->shader, "light.specular", (vec3){ 1.0, 1.0, 1.0 });

    shader_set_uniform_vec3(player->shader,  "material.ambient",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(player->shader,  "material.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(player->shader,  "material.specular", (vec3){ 0.5, 0.5, 0.5  });
    shader_set_uniform_float(player->shader, "material.shininess", 32.0f);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

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

    vec3 vel_offset, tmp;
    glm_vec3_zero(vel_offset);

    if (key_is_down('W')) {
        glm_vec3_copy(player->camera.dir, tmp);
        tmp[1] = 0;
        glm_vec3_normalize(tmp);
        glm_vec3_add(vel_offset, tmp, vel_offset);
    }
    if (key_is_down('S')) {
        glm_vec3_copy(player->camera.dir, tmp);
        tmp[1] = 0;
        glm_vec3_negate(tmp);
        glm_vec3_normalize(tmp);
        glm_vec3_add(vel_offset, tmp, vel_offset);
    }
    if (key_is_down('A')) {
        glm_vec3_copy(player->camera.right, tmp);
        glm_vec3_normalize(tmp);
        glm_vec3_add(vel_offset, tmp, vel_offset);
    }
    if (key_is_down('D')) {
        glm_vec3_copy(player->camera.right, tmp);
        glm_vec3_negate(tmp);
        glm_vec3_normalize(tmp);
        glm_vec3_add(vel_offset, tmp, vel_offset);
    }

    /* if (key_is_down('Q')) { */
    /*     glm_vec3_copy((vec3){0, -1, 0 }, tmp); */
    /*     glm_vec3_normalize(tmp); */
    /*     glm_vec3_add(vel_offset, tmp, vel_offset); */
    /* } */
    /* if (key_is_down('E')) { */
    /*     glm_vec3_copy((vec3){0, 1, 0 }, tmp); */
    /*     glm_vec3_normalize(tmp); */
    /*     glm_vec3_add(vel_offset, tmp, vel_offset); */
    /* } */

    glm_vec3_copy(vel_offset, player->vel);

    // FIXME: jumping
    static const float jumpvel = 1.0f;
    if (player->grounded && key_pressed('E')) {
        player->vel[1] += jumpvel;
        player->grounded = false;
    }
}

bool player_collides_with_world_at_pos(Player *player, vec3 nextpos)
{
    Bbox bbox = make_bbox( 
        nextpos[0] - player->width/2, 
        nextpos[1] - player->height/2, 
        nextpos[2] - player->width/2,
        nextpos[0] + player->width/2, 
        nextpos[1] + player->height/2, 
        nextpos[2] + player->width/2 
    );

    vec3 v1, v2, v3, v4, v5, v6, v7, v8;
    glm_vec3_copy(bbox.min, v1);
    glm_vec3_copy(bbox.min, v2);
    glm_vec3_copy(bbox.min, v3);
    glm_vec3_copy(bbox.min, v4);
    glm_vec3_copy(bbox.max, v5);
    glm_vec3_copy(bbox.max, v6);
    glm_vec3_copy(bbox.max, v7);
    glm_vec3_copy(bbox.max, v8);

    v2[0] += player->width;

    v3[1] += player->height;

    v4[2] += player->width;

    v5[0] += player->width;
    v5[1] += player->height;

    v6[1] += player->height;
    v6[2] += player->width;

    v7[0] += player->width;
    v7[2] += player->width;

    v8[0] += player->width;
    v8[1] += player->height;
    v8[2] += player->width;

    Block b1 = world_blockat_worldpos(player->world, v1);
    Block b2 = world_blockat_worldpos(player->world, v2);
    Block b3 = world_blockat_worldpos(player->world, v3);
    Block b4 = world_blockat_worldpos(player->world, v4);
    Block b5 = world_blockat_worldpos(player->world, v5);
    Block b6 = world_blockat_worldpos(player->world, v6);
    Block b7 = world_blockat_worldpos(player->world, v7);
    Block b8 = world_blockat_worldpos(player->world, v8);

    return b1.type != BLOCK_AIR || b2.type != BLOCK_AIR || b3.type != BLOCK_AIR || b4.type != BLOCK_AIR
        || b5.type != BLOCK_AIR || b6.type != BLOCK_AIR || b7.type != BLOCK_AIR || b8.type != BLOCK_AIR;
}

void player_send_to_ground(Player *player) 
{
    vec3 v1, v2, v3, v4;
    glm_vec3_zero(v1);
    glm_vec3_zero(v2);
    glm_vec3_zero(v3);
    glm_vec3_zero(v4);

    v1[1] -= player->height/2; v2[1] -= player->height/2; 
    v3[1] -= player->height/2; v4[1] -= player->height/2; 

    v1[0] -= player->width/2;  v1[2] -= player->width/2;
    v2[0] -= player->width/2;  v2[2] += player->width/2;
    v3[0] += player->width/2;  v3[3] += player->width/2;
    v4[0] += player->width/2;  v4[3] -= player->width/2;

    mat4 rot0, rot1;
    glm_rotate_make(rot0, player->camera.yaw, (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate_make(rot1, -player->camera.yaw, (vec3){0.0f, 1.0f, 0.0f});

    glm_mat4_mulv3(rot0, v1, 1.0f, v1);
    glm_mat4_mulv3(rot0, v2, 1.0f, v2);
    glm_mat4_mulv3(rot0, v3, 1.0f, v3);
    glm_mat4_mulv3(rot0, v4, 1.0f, v4);

    glm_vec3_add(player->pos, v1, v1);
    glm_vec3_add(player->pos, v2, v2);
    glm_vec3_add(player->pos, v3, v3);
    glm_vec3_add(player->pos, v4, v4);

    while (true) {
        Block b1 = world_blockat_worldpos(player->world, v1);
        Block b2 = world_blockat_worldpos(player->world, v2);
        Block b3 = world_blockat_worldpos(player->world, v3);
        Block b4 = world_blockat_worldpos(player->world, v4);

        if (b1.type != BLOCK_AIR) {
            glm_mat4_mulv3(rot1, v1, 1.0f, v1);
            vec3 blockpos;
            worldpos_from_blockworldpos(blockworldpos_from_worldpos(v1), blockpos);
            v1[1] += BLOCK_SIDELEN - fmodf(v1[1], BLOCK_SIDELEN) + player->height/2;
            v1[0] += player->width/2;  v1[2] += player->width/2;
            glm_vec3_copy(v1, player->pos);
            break;
        } else if (b2.type != BLOCK_AIR) {
            glm_mat4_mulv3(rot1, v2, 1.0f, v2);
            vec3 blockpos;
            worldpos_from_blockworldpos(blockworldpos_from_worldpos(v2), blockpos);
            v2[1] += BLOCK_SIDELEN - fmodf(v2[2], BLOCK_SIDELEN) + player->height/2;
            v2[0] += player->width/2;  v2[2] -= player->width/2;
            glm_vec3_copy(v2, player->pos);
            break;
        } else if (b3.type != BLOCK_AIR) {
            glm_mat4_mulv3(rot1, v3, 1.0f, v3);
            vec3 blockpos;
            worldpos_from_blockworldpos(blockworldpos_from_worldpos(v3), blockpos);
            v3[1] += BLOCK_SIDELEN - fmodf(v3[3], BLOCK_SIDELEN) + player->height/2;
            v3[0] -= player->width/2;  v3[3] -= player->width/2;
            glm_vec3_copy(v3, player->pos);
            break;
        } else if (b4.type != BLOCK_AIR) {
            glm_mat4_mulv3(rot1, v4, 1.0f, v4);
            vec3 blockpos;
            worldpos_from_blockworldpos(blockworldpos_from_worldpos(v4), blockpos);
            v4[1] += BLOCK_SIDELEN - fmodf(v4[4], BLOCK_SIDELEN) + player->height/2;
            v4[0] -= player->width/2;  v4[3] += player->width/2;
            glm_vec3_copy(v4, player->pos);
            break;
        } else {
            v1[1] -= BLOCK_SIDELEN; v2[1] -= BLOCK_SIDELEN;
            v3[1] -= BLOCK_SIDELEN; v4[1] -= BLOCK_SIDELEN;
        }
    }
}

void player_update(Player *player, float dt)
{
    assert(player);

    vec3 vel, nextpos, gravpos;
    float yvel_off;

    if (!player->grounded) {
        glm_vec3_copy(player->pos, gravpos);

        static const float gravity = 0.015f;
        yvel_off = player->vel[1];
        yvel_off -= gravity * dt;
        gravpos[1] += yvel_off;

        if (!player_collides_with_world_at_pos(player, gravpos)) {
            player->vel[1] += yvel_off;
        } else {
            player_send_to_ground(player);
            player->grounded = true;
        }
    }

    static const float move_speed = 0.002f;
    glm_vec3_scale(player->vel, move_speed * dt, vel);
    glm_vec3_add(player->pos, vel, nextpos);
    if (!player_collides_with_world_at_pos(player, nextpos)) {
        glm_vec3_copy(nextpos, player->pos);
    }

    vec3 campos;
    player_compute_camera_pos(player, campos);
    glm_vec3_copy(campos, player->camera.pos);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, player->pos);
    glm_rotate_y(model, player->camera.yaw, model);

    camera_compute_view_and_projection(&player->camera);
    shader_set_uniform_mat4(player->shader, "model", model);
    shader_set_uniform_mat4(player->shader, "view",  player->camera.view);
    shader_set_uniform_mat4(player->shader, "projection", player->camera.projection);
    shader_set_uniform_vec3(player->shader, "viewPos", player->camera.pos);

    player_compute_bbox(player);
}

void player_compute_camera_pos(Player *player, vec3 result)
{
    // rodridgues' formula for rotating vectors
    // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula

    vec3 cam_off, tmp0, tmp1, tmp2;
    float a = glm_rad(-15);
    static const float camera_distance = 3.0f;

    glm_vec3_copy(player->camera.dir, tmp0);
    glm_vec3_normalize(tmp0);
    glm_vec3_copy(player->camera.right, tmp1);
    glm_vec3_normalize(tmp1);
    glm_vec3_copy(tmp1, tmp2);

    glm_vec3_scale(tmp0, cos(a), tmp0);

    glm_vec3_cross(player->camera.dir, tmp1, tmp1);
    glm_vec3_scale(tmp1, sin(a), tmp1);

    float dot = glm_vec3_dot(player->camera.right, player->camera.dir);
    glm_vec3_scale(tmp2, dot*(1.0f - cos(a)), tmp2);

    glm_vec3_add(tmp2, tmp1, tmp2);
    glm_vec3_add(tmp2, tmp0, tmp2);

    glm_vec3_copy(tmp2, cam_off);
    glm_vec3_negate(cam_off);
    glm_vec3_scale(cam_off, camera_distance, cam_off);
    glm_vec3_add(player->pos, cam_off, cam_off);

    glm_vec3_copy(cam_off, result);
}

void player_render(Player *player, float dt)
{
    assert(player);

    UNUSED(dt);

    glUseProgram(player->shader);
    glBindVertexArray(player->vao);
    glBindBuffer(GL_ARRAY_BUFFER, player->vbo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void player_compute_bbox(Player *player)
{
    assert(player);

    player->bbox = make_bbox( 
        player->pos[0] - player->width/2, 
        player->pos[1] - player->height/2, 
        player->pos[2] - player->width/2,
        player->pos[0] + player->width/2, 
        player->pos[1] + player->height/2, 
        player->pos[2] + player->width/2 
    );
}

PlayerModelVertex make_player_model_vertex(float x, float y, float z, 
                                           float nx, float ny, float nz, 
                                           float r, float g, float b, float a)
{
    return (PlayerModelVertex) { { x, y, z }, { nx, ny, nz }, { r, g, b, a } };
}

void player_load_vertices(Player *player)
{
    assert(player);

    const float X = player->width/2.0f;
    const float Y = player->height/2.0f;
    const float Z = player->width/2.0f;

    da_append(&player->vertices, make_player_model_vertex( -X, -Y, -X,    0,  0, -1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y, -X,    0,  0, -1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y, -X,    0,  0, -1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y, -X,    0,  0, -1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y, -X,    0,  0, -1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y, -X,    0,  0, -1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y,  X,    0,  0,  1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y,  X,    0,  0,  1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y,  X,    0,  0,  1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y,  X,    0,  0,  1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y,  X,    0,  0,  1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y,  X,    0,  0,  1,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y,  X,   -1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y, -X,   -1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y, -X,   -1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y, -X,   -1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y,  X,   -1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y,  X,   -1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y,  X,    1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y, -X,    1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y, -X,    1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y, -X,    1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y,  X,    1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y,  X,    1,  0,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y, -X,    0, -1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y, -X,    0, -1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y,  X,    0, -1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X, -Y,  X,    0, -1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y,  X,    0, -1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X, -Y, -X,    0, -1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y, -X,    0,  1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y,  X,    0,  1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y, -X,    0,  1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex(  X,  Y,  X,    0,  1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y, -X,    0,  1,  0,   1, 1, 1, 1 ));
    da_append(&player->vertices, make_player_model_vertex( -X,  Y,  X,    0,  1,  0,   1, 1, 1, 1 ));
}

