#include "chunkmanager.h"

#include "world.h"
#include "player.h"

int chunkmanager_init(ChunkManager *manager, World *world)
{
    assert(manager);

    manager->world = world;

    if (0 > shader_create(&manager->shader, "src/glsl/chunk_vs.glsl", "src/glsl/chunk_fs.glsl")) {
        fprintf(stderr, "Failed to create chunk shader.\n");
        return -1;
    }

    glUseProgram(manager->shader);

    glGenVertexArrays(1, &manager->vao);
    glGenBuffers(1, &manager->vbo);

    glBindVertexArray(manager->vao);
    glBindBuffer(GL_ARRAY_BUFFER, manager->vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(BlockVertex),
                          (GLvoid *) offsetof(BlockVertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(BlockVertex),
                          (GLvoid *) offsetof(BlockVertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                          sizeof(BlockVertex),
                          (GLvoid *) offsetof(BlockVertex, color));
    /* glEnableVertexAttribArray(3); */
    /* glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, */
    /*                       sizeof(GLint), */
    /*                       (GLvoid *) 0); */
    glEnableVertexAttribArray(0);

    shader_set_uniform_vec3(manager->shader, "light.color", &manager->world->lightsource.color.raw[0]);
    shader_set_uniform_vec3(manager->shader, "light.pos",   &manager->world->lightsource.pos.raw[0]);
    shader_set_uniform_vec3(manager->shader, "light.ambient",  (vec3){ 0.2, 0.2, 0.2 });
    shader_set_uniform_vec3(manager->shader, "light.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader, "light.specular", (vec3){ 1.0, 1.0, 1.0 });

    shader_set_uniform_vec3(manager->shader,  "material.ambient",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader,  "material.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader,  "material.specular", (vec3){ 0.5, 0.5, 0.5  });
    shader_set_uniform_float(manager->shader, "material.shininess", 32.0f);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return 0;
}

void chunkmanager_deinit(ChunkManager *manager)
{
    assert(manager);

    for (int i = 0; i < (int)manager->chunks.count; ++i) {
        chunk_delete(&manager->chunks.items[i]);
    }

    glDeleteBuffers(1, &manager->vbo);
    glDeleteVertexArrays(1, &manager->vao);
    glDeleteProgram(manager->shader);
}

void chunkmanager_update(ChunkManager *manager, float dt)
{
    assert(manager);

    for (int i = 0; i < (int)manager->chunks.count; ++i) {
        chunk_update(&manager->chunks.items[i], dt);
    }
}

void chunkmanager_render(ChunkManager *manager, float dt)
{
    assert(manager);

    shader_set_uniform_mat4(manager->shader, 
                            "projection",
                            manager->world->player->camera.projection);
    shader_set_uniform_mat4(manager->shader, 
                            "view",
                            manager->world->player->camera.view);
    shader_set_uniform_vec3(manager->shader, 
                            "viewPos", 
                            manager->world->player->camera.pos);

    for (int i = 0; i < (int)manager->chunks.count; ++i) {
        chunk_render(&manager->chunks.items[i], dt);
    }
}

Chunk* chunkmanager_get_chunk(ChunkManager *manager, ChunkId id)
{
    assert(manager);

    for (int i = 0; i < (int)manager->chunks.count; ++i) {
        if (id.x == manager->chunks.items[i].id.x 
            && id.y == manager->chunks.items[i].id.y
            && id.z == manager->chunks.items[i].id.z) 
        {
            return &manager->chunks.items[i];
        }
    }

    verr("tried to get chunk that doesn't exist: (%d, %d, %d)", id.x, id.y, id.z);
    __builtin_trap();
    cleanup_and_exit(1);
    __builtin_unreachable();
    return NULL;
}
