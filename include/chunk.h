#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "block.h"

typedef struct BlockVertex {
    vec3      pos;
    vec3      normal;
    vec4      color;
    BlockType type;
} BlockVertex;

#define CHUNK_SZ (32)
#define BLOCK_SIDELEN (1.0f)
#define CHUNK_SIDELEN ((float)CHUNK_SZ*BLOCK_SIDELEN)

typedef struct World World;

typedef struct Chunk {
    ivec3s id;
    World *world;

    Block ***blocks;

    Shader shader;
    GLuint vao, vbo;
    list(BlockVertex, vertices);
    mat4 model;

    bool dirty;
    bool loaded;
} Chunk;

int chunk_new(Chunk *chunk, ivec3s pos, World *world, Shader shader);
void chunk_delete(Chunk *chunk);

void chunk_update(Chunk *chunk, float dt);
void chunk_render(Chunk *chunk, float dt);

void chunk_load(Chunk *chunk);
void chunk_rebuild_mesh(Chunk *chunk);

void chunk_put_block(Chunk *chunk, Block block, ivec3s pos);

#endif
