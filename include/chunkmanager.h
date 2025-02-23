#ifndef CHUNK_MANAGER_H

#define CHUNK_MANAGER_H

#include "glfw.h"
#include "common.h"
#include "shader.h"
#include "chunk.h"

typedef struct BlockPos {
    ivec2s chunkid;
    ivec3s blockpos;
} BlockPos;

typedef struct World World;

typedef struct ChunkManager {
    GLuint vao, vbo;
    Shader shader;
    struct {
        Chunk *items;
        size_t count;
        size_t capacity;
    } chunks;
    World *world;
} ChunkManager;

int chunkmanager_init(ChunkManager *manager, World *world);
void chunkmanager_deinit(ChunkManager *manager);

void chunkmanager_update(ChunkManager *manager, float dt);
void chunkmanager_render(ChunkManager *manager, float dt);

Chunk* chunkmanager_get_chunk(ChunkManager *manager, ChunkId id);

#endif
