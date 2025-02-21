#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "block.h"

#define MAX_CHUNK_X (127)
#define MIN_CHUNK_X (0)
#define MAX_CHUNK_Z (127)
#define MIN_CHUNK_Z (0)

typedef struct BlockVertex {
    vec3      pos;
    vec3      normal;
    vec4      color;
    BlockType type;
} BlockVertex;

typedef struct ChunkMesh {
} ChunkMesh;

#define CHUNK_SZ (32)

typedef struct World World;

typedef struct Chunk {
    ivec2s pos;
    ChunkMesh mesh;
    bool dirty;
    Block ***blocks;
    World *world;
    Shader shader;
    GLuint vao, vbo;
    struct {
        BlockVertex *items;
        size_t count;
        size_t capacity;
    } vertices;
} Chunk;

int chunk_new(Chunk *chunk, ivec2s pos, World *world, Shader shader, GLuint vao, GLuint vbo);
void chunk_delete(Chunk *chunk);
void chunk_rebuild_mesh(Chunk *chunk);
void chunk_update(Chunk *chunk, float dt);
void chunk_render(Chunk *chunk, float dt);
void chunk_put_block(Chunk *chunk, Block block, ivec3s pos);

int chunkmesh_new(ChunkMesh *mesh);
void chunkmesh_delete(ChunkMesh *mesh);

#endif
