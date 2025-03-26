#include "world.h"

#include "common.h"

#include "noise.h"

Chunk make_chunk(World *world, ivec3s chunkpos);

void world_prepare(World *world)
{
    int seed = rand();
    int r = rand() % 40;
    for (int cx = -6; cx <= 6; ++cx) {
        for (int cz = -6; cz <= 6; ++cz) {
            for (int x = 0; x < CHUNK_SZ; ++x) {
                for (int z = 0; z < CHUNK_SZ; ++z) {
                    double nx = cx*CHUNK_SZ + x;
                    double nz = cz*CHUNK_SZ + z;

                    double n = noise2d(0.05*(CHUNK_SZ*nx+x), 0.05*(CHUNK_SZ*nz+z));
                    /* n += .2; */
                    n = fade(n);
                    n = pow(n, 4.0);
                    printf("n = %lf\n", n);

                    /* n = clamp(tanh(n*1.25), 0, 1); */
                    /* n = glm_smoothstep(0, 1, n); */

                    static const int ymax = 10*CHUNK_SZ;
                    int yn = (int) clamp(floorf(ymax*n), 0, ymax-1);

                    const int watercutoff = 12;
                    if (yn <= watercutoff) {
                        for (int y = 0; y < watercutoff; ++y) {
                            put_block(world, (Block){ BLOCK_WATER },
                                      (BlockWorldPos) {
                                          .chunk=(ivec3s){ .x=cx, .y=0, .z=cz },
                                          .block=(ivec3s){ .x=x,  .y=y, .z=z  }});
                        }
                    } else {
                        for (int y = 0; y < yn; ++y) {
                            put_block(world, (Block){ BLOCK_GRASS }, 
                                      (BlockWorldPos) {
                                          .chunk=(ivec3s){ .x=cx, .y=y/CHUNK_SZ, .z=cz },
                                          .block=(ivec3s){ .x=x,  .y=y%CHUNK_SZ, .z=z  }});
                        }
                    }
                }
            }
        }
    }
}

int world_init(World *world, Player *player)
{
    assert(world);

    world->player = player;

    const float radius = 0.5f;
    const vec3s pos = (vec3s){ .x=100, .y=200, .z=100 };
    const vec3s color = (vec3s){ .r=1, .g=1, .b=1 };
    if (0 > lightsource_init(&world->lightsource, radius, pos, color, world)) {
        err("Failed to initialize light source.");
        return -1;
    }

    if (0 > chunkmanager_init(&world->chunkmanager, world)) {
        err("Failed to initialize chunk manager.");
        return -2;
    }

    world_prepare(world);

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

    Chunk *chunk = get_chunk(&world->chunkmanager, pos.chunk);
    if (!chunk) {
        verr("tried to get block at position where chunk doesn't exist "
             "[chunkpos=(%0.3f,%0.3f),blockpos=(%0.3f,%0.3f,%0.3f)]", 
             pos.chunk.x,pos.chunk.y,pos.block.x,pos.block.y,pos.block.z);
        __builtin_trap();
        cleanup_and_exit(2);
    }

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
    result[1] = BLOCK_SIDELEN*pos.block.y;
    result[2] = CHUNK_SIDELEN*pos.chunk.y + BLOCK_SIDELEN*pos.block.z;
}

Chunk make_chunk(World *world, ivec3s chunkpos)
{
    assert(world);

    Chunk chunk;
    if (0 > chunk_new(&chunk, chunkpos, world, world->chunkmanager.shader)) {
        err("Failed to initialize chunk.");
        cleanup_and_exit(2);
    }

    return chunk;
}

void put_block(World *world, Block block, BlockWorldPos pos)
{
    Chunk *chunk;
    if (!(chunk = get_chunk(&world->chunkmanager, pos.chunk))) {
        Chunk chunk = make_chunk(world, pos.chunk);
        chunk_put_block(&chunk, block, pos.block);
        list_append(&world->chunkmanager.chunks, chunk);
        return;
    }
    chunk_put_block(chunk, block, pos.block);
}

