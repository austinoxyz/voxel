#ifndef WORLD_H
#define WORLD_H

#include "lightsource.h"
#include "chunkmanager.h"

typedef struct Player Player;

typedef struct BlockWorldPos {
    ivec3s chunk;
    ivec3s block;
} BlockWorldPos;

typedef struct World {
    LightSource  lightsource;
    ChunkManager chunkmanager;
    Player *player;
} World;

int world_init(World *world, Player *player);
void world_deinit(World *world);

void world_update(World *world, float dt);
void world_render(World *world, float dt);

Block world_blockat_worldpos(World *world, vec3 worldpos);
Block world_blockat(World *world, BlockWorldPos pos);

BlockWorldPos blockworldpos_from_worldpos(vec3 worldpos);
void worldpos_from_blockworldpos(BlockWorldPos pos, vec3 result);

void put_block(World *world, Block block, BlockWorldPos pos);

#endif
