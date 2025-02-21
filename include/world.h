#ifndef WORLD_H
#define WORLD_H

#include "lightsource.h"
#include "chunkmanager.h"

typedef struct World {
    LightSource  lightsource;
    ChunkManager chunkmanager;
} World;

int world_init(World *world);
void world_deinit(World *world);

void world_update(World *world, float dt);
void world_render(World *world, float dt);

#endif
