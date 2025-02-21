#include "chunkmanager.h"

#include "world.h"

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

    mat4 model;
    glm_mat4_identity(model);
    shader_set_uniform_mat4(manager->shader, "model", model);

    shader_set_uniform_vec3(manager->shader, "light.color", &manager->world->lightsource.color.raw[0]);
    shader_set_uniform_vec3(manager->shader, "light.pos",   &manager->world->lightsource.pos.raw[0]);
    shader_set_uniform_vec3(manager->shader, "light.ambient",  (vec3){ 0.2, 0.2, 0.2 });
    shader_set_uniform_vec3(manager->shader, "light.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader, "light.specular", (vec3){ 1.0, 1.0, 1.0 });

    shader_set_uniform_vec3(manager->shader,  "material.ambient",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader,  "material.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader,  "material.specular", (vec3){ 0.5, 0.5, 0.5  });
    shader_set_uniform_float(manager->shader, "material.shininess", 32.0f);

    // tmp code
    //------------------------------------------
    if (0 > chunk_new(&manager->chunk, (ivec2s){ .x=0, .y=0 }, world, manager->shader, manager->vao, manager->vbo)) {
        err("Failed to initialize chunk.");
        return -2;
    }
    const int N = BLOCK_TYPE_COUNT;
    for (int i = 0; i < 5; ++i) {
        for (BlockType type = BLOCK_DIRT; (int) type < N; ++type) {
            chunk_put_block(&manager->chunk, 
                            (Block){ 1 + ((type + i) % (N-1)) }, 
                            (ivec3s){ .x=i, .y=(type-1), .z=0 });
        }
    }
    //------------------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return 0;
}

void chunkmanager_deinit(ChunkManager *manager)
{
    assert(manager);

    chunk_delete(&manager->chunk);

    glDeleteBuffers(1, &manager->vbo);
    glDeleteVertexArrays(1, &manager->vao);
    glDeleteProgram(manager->shader);
}

void chunkmanager_update(ChunkManager *manager, float dt)
{
    assert(manager);

    chunk_update(&manager->chunk, dt);
}

void chunkmanager_render(ChunkManager *manager, float dt)
{
    assert(manager);

    chunk_render(&manager->chunk, dt);
}
