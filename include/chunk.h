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

typedef ivec3s ChunkId;

typedef struct Chunk {
    ChunkId id;
    bool dirty;
    Block ***blocks;
    World *world;
    Shader shader;
    GLuint vao, vbo;
    mat4 model;
    struct {
        BlockVertex *items;
        size_t count;
        size_t capacity;
    } vertices;
} Chunk;

int chunk_new(Chunk *chunk, ChunkId pos, World *world, Shader shader, GLuint vao, GLuint vbo);
void chunk_delete(Chunk *chunk);
void chunk_rebuild_mesh(Chunk *chunk);
void chunk_update(Chunk *chunk, float dt);
void chunk_render(Chunk *chunk, float dt);
void chunk_put_block(Chunk *chunk, Block block, ivec3s pos);

#endif
