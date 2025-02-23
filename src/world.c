#include "world.h"

#include "common.h"

int world_init(World *world, Player *player)
{
    assert(world);

    world->player = player;

    const float radius = 0.5f;
    const vec3s pos = (vec3s){ .x=2.5, .y=2.5, .z=3 };
    const vec3s color = (vec3s){ .r=1, .g=1, .b=1 };
    if (0 > lightsource_init(&world->lightsource, radius, pos, color, world)) {
        err("Failed to initialize light source.");
        return -1;
    }

    if (0 > chunkmanager_init(&world->chunkmanager, world)) {
        err("Failed to initialize chunk manager.");
        return -2;
    }

    for (int x = -4; x <= 4; ++x) {
        for (int y = -4; y <= 4; ++y) {
            for (int z = -4; z <= 4; ++z) {
                Chunk chunk;
                if (0 > chunk_new(&chunk, (ChunkId){ .x=x, .y=y, .z=z }, 
                                  world, world->chunkmanager.shader, 
                                  world->chunkmanager.vao, world->chunkmanager.vbo)) 
                {
                    err("Failed to initialize chunk.");
                    return -2;
                }
                if (y == 0) {
                    for (int x2 = 0; x2 < (int)CHUNK_SZ; ++x2) {
                        for (int z2 = 0; z2 < (int)CHUNK_SZ; ++z2) {
                            chunk_put_block(&chunk, (Block){ BLOCK_STONE }, 
                                            (ivec3s){ .x=x2, .y=0, .z=z2 });
                        }
                    }
                }
                da_append(&world->chunkmanager.chunks, chunk);
            }
        }
    }

    return 0;
}

void world_deinit(World *world)
{
    assert(world);

    lightsource_deinit(&world->lightsource);
    chunkmanager_deinit(&world->chunkmanager);
}

void world_update(World *world, float dt)
{
    assert(world);

    lightsource_update(&world->lightsource, dt);
    chunkmanager_update(&world->chunkmanager, dt);
}

void world_render(World *world, float dt)
{
    assert(world);

    lightsource_render(&world->lightsource, dt);
    chunkmanager_render(&world->chunkmanager, dt);
}

Block world_blockat_worldpos(World *world, vec3 worldpos)
{
    assert(world);

    BlockWorldPos pos = blockworldpos_from_worldpos(worldpos);
    return world_blockat(world, pos);
}

Block world_blockat(World *world, BlockWorldPos pos)
{
    assert(world);

    Chunk *chunk = chunkmanager_get_chunk(&world->chunkmanager, pos.chunk);
    return chunk->blocks[pos.block.x][pos.block.y][pos.block.z];
}

BlockWorldPos blockworldpos_from_worldpos(vec3 worldpos)
{
    ivec3s id;
    id.x = (int) floor(worldpos[0] / CHUNK_SIDELEN);
    id.y = (int) floor(worldpos[1] / CHUNK_SIDELEN);
    id.z = (int) floor(worldpos[2] / CHUNK_SIDELEN);

    #define mod(a, b) (fmodf(fmodf((a), (b)) + (b), (b)))

    ivec3s blockpos;
    blockpos.x = (int) (mod(worldpos[0], CHUNK_SIDELEN) / BLOCK_SIDELEN);
    blockpos.y = (int) (mod(worldpos[1], CHUNK_SIDELEN) / BLOCK_SIDELEN);
    blockpos.z = (int) (mod(worldpos[2], CHUNK_SIDELEN) / BLOCK_SIDELEN);

    #undef mod

    return (BlockWorldPos) { .chunk=id, .block=blockpos };
}

void worldpos_from_blockworldpos(BlockWorldPos pos, vec3 result) 
{
    result[0] = CHUNK_SIDELEN*pos.chunk.x + BLOCK_SIDELEN*pos.block.x;
    result[1] = CHUNK_SIDELEN*pos.chunk.y + BLOCK_SIDELEN*pos.block.y;
    result[2] = CHUNK_SIDELEN*pos.chunk.z + BLOCK_SIDELEN*pos.block.z;
}


