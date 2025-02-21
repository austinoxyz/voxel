#ifndef PLAYER_H
#define PLAYER_H

#include "glfw.h"
#include "common.h"
#include "camera.h"

typedef struct World World;

typedef struct Player {
    Camera camera;
    vec3 pos;
    vec3 vel;
    World *world;
} Player;

int player_init(Player *player, World *world);
void player_deinit(Player *player);

void player_handle_input(Player *player, float dt);
void player_update(Player *player, float dt);
void player_render(Player *player, float dt);

#endif
