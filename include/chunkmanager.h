#ifndef CHUNK_MANAGER_H

#define CHUNK_MANAGER_H

#include "glfw.h"
#include "common.h"
#include "shader.h"
#include "chunk.h"

typedef struct World World;

static const float RENDER_DISTANCE = 100.0f;

typedef struct ChunkManager {
    Shader shader;
    World *world;
    list(Chunk, chunks);
    list(Chunk*, renderlist);
    list(Chunk*, loadlist);
    list(Chunk*, unloadlist);
} ChunkManager;

int chunkmanager_init(ChunkManager *manager, World *world);
void chunkmanager_deinit(ChunkManager *manager);

void chunkmanager_update(ChunkManager *manager, float dt);
void chunkmanager_render(ChunkManager *manager, float dt);

Chunk* get_chunk(ChunkManager *manager, ivec3s chunkpos);

ivec3s chunkpos_from_worldpos(vec3 worldpos);

#endif
