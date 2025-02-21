#include "world.h"

int world_init(World *world)
{
    assert(world);

    const float radius = 0.5f;
    const vec3s pos = (vec3s){ .x=2.5, .y=2.5, .z=-3 };
    const vec3s color = (vec3s){ .r=1, .g=1, .b=1 };
    if (0 > lightsource_init(&world->lightsource, radius, pos, color)) {
        err("Failed to initialize light source.");
        return -1;
    }

    if (0 > chunkmanager_init(&world->chunkmanager, world)) {
        err("Failed to initialize chunk manager.");
        return -2;
    }

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
