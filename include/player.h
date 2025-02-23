#ifndef PLAYER_H
#define PLAYER_H

#include "glfw.h"
#include "common.h"
#include "camera.h"
#include "bbox.h"

typedef struct World World;

typedef struct PlayerModelVertex {
    vec3 pos;
    vec3 normal;
    vec4 color;
} PlayerModelVertex;

typedef struct Player {
    GLuint vao, vbo;
    Shader shader;
    Camera camera;
    vec3 pos;
    vec3 vel;
    Bbox bbox;
    float width;
    float height;
    struct {
        PlayerModelVertex *items;
        size_t count;
        size_t capacity;
    } vertices;
    World *world;
    bool grounded;
} Player;

int player_init(Player *player, World *world);
void player_deinit(Player *player);

void player_handle_input(Player *player, float dt);
void player_update(Player *player, float dt);
void player_render(Player *player, float dt);

#endif
