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

    shader_set_uniform_vec3(manager->shader, "light.color", &manager->world->lightsource.color.raw[0]);
    shader_set_uniform_vec3(manager->shader, "light.pos",   &manager->world->lightsource.pos.raw[0]);
    shader_set_uniform_vec3(manager->shader, "light.ambient",  (vec3){ 0.2, 0.2, 0.2 });
    shader_set_uniform_vec3(manager->shader, "light.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader, "light.specular", (vec3){ 1.0, 1.0, 1.0 });

    shader_set_uniform_vec3(manager->shader,  "material.ambient",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader,  "material.diffuse",  (vec3){ 0.5, 0.5, 0.5 });
    shader_set_uniform_vec3(manager->shader,  "material.specular", (vec3){ 0.5, 0.5, 0.5  });
    shader_set_uniform_float(manager->shader, "material.shininess", 32.0f);

    glUseProgram(0);

    return 0;
}

void chunkmanager_deinit(ChunkManager *manager)
{
    assert(manager);

    for (int i = 0; i < (int)manager->chunks.count; ++i) {
        chunk_delete(&manager->chunks.items[i]);
    }

    glDeleteProgram(manager->shader);
}

void chunkmanager_update(ChunkManager *manager, float dt)
{
    assert(manager);

    // TODO: Sort manager->chunks and build 
    //       manager->loadlist and manager->unloadlist

    /* for (int i = 0; i < (int)manager->loadlist.count; ++i) { */
    /*     chunk_load(&manager->loadlist.items[i]); */
    /* } */
    /* for (int i = 0; i < (int)manager->unloadlist.count; ++i) { */
    /*     chunk_unload(&manager->unloadlist.items[i]); */
    /* } */

    /* manager->loadlist.count = 0; */
    /* manager->unloadlist.count = 0; */

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

Chunk* get_chunk(ChunkManager *manager, ivec3s id)
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

    return NULL;
}

ivec3s chunkpos_from_worldpos(vec3 worldpos)
{
    return (ivec3s) { 
        .x=((int) worldpos[0] / CHUNK_SZ),
        .y=((int) worldpos[1] / CHUNK_SZ),
        .z=((int) worldpos[2] / CHUNK_SZ),
    };
}
