#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "glfw.h"
#include "common.h"
#include "shader.h"
#include "chunk.h"

typedef struct World World;

typedef struct ChunkManager {
    GLuint vao, vbo;
    Shader shader;
    Chunk chunk;
    World *world;
} ChunkManager;

int chunkmanager_init(ChunkManager *manager, World *world);
void chunkmanager_deinit(ChunkManager *manager);

void chunkmanager_update(ChunkManager *manager, float dt);
void chunkmanager_render(ChunkManager *manager, float dt);

#endif
