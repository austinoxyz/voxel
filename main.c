#include <stdio.h>

#include "window.h"
#include "camera.h"
#include "common.h"
#include "chunk.h"
#include "input.h"
#include "lightsource.h"

const ivec2s s_winsize = (ivec2s){ .x=800, .y=600 };
const float  s_aspect = (float) s_winsize.x / (float) s_winsize.y;
const char *s_window_title = "opengl-voxel";

static Chunk s_chunk;

void cleanup()
{
    deinit_chunk_renderer();
    lightsource_deinit();
    window_destroy();
    glfw_close();
}

void init()
{
    set_cleanup_function(cleanup);

    glfw_init();

    if (0 > window_create(s_winsize, s_window_title)) {
        err("Failed to create window.");
        cleanup_and_exit(1);
    }

    input_handler_init();

    camera_init((vec3){ 0, 0, 3 }, (vec3){ 0, 0, -1 }, 45.0f);

    static const vec3s lightsource_pos = (vec3s){ .x=-2, .y=5, .z=-3 };
    static const vec3s lightsource_color = (vec3s){ .r=0.959f, .g=0.809f, .b=0.809f };
    if (0 > lightsource_init(0.5f, lightsource_pos, lightsource_color)) {
        err("Failed to initialize light source.");
        cleanup_and_exit(1);
    }

    if (0 > init_chunk_renderer()) {
        err("Failed to initialize chunk renderer.");
        cleanup_and_exit(1);
    }

    if (0 > chunk_new(&s_chunk, (ivec2s){ .x=0, .y=0 })) {
        err("Failed to initialize chunk.");
        cleanup_and_exit(1);
    }

    chunk_put_block(&s_chunk, (Block){ BLOCK_STONE }, (ivec3s){ .x=0, .y=0, .z=0 });
    chunk_put_block(&s_chunk, (Block){ BLOCK_DIRT },  (ivec3s){ .x=0, .y=1, .z=0 });
    chunk_put_block(&s_chunk, (Block){ BLOCK_GRASS }, (ivec3s){ .x=0, .y=2, .z=0 });

    info("Initialized successfully.");
}

void update(float dt)
{
    input_handler_poll();

    camera_update(dt);
    chunk_update(&s_chunk, dt);
    lightsource_update(dt);
}

void render(float dt)
{
    UNUSED(dt);
    glClearColor(0.2, 0.2, 0.2, 0.8);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightsource_render();
        chunk_render(&s_chunk);
    glfwSwapBuffers(window_get_handle());
    glfwPollEvents();
}

int main(void) 
{
    init();

    long acc_time, end_time, start_time, dt;
    end_time = start_time = get_time_us();
    acc_time = dt = 0;

    while (!glfwWindowShouldClose(window_get_handle())) 
    {
        update(dt);
        render(dt);

        end_time = get_time_us();
        dt = (end_time - start_time) / 1000.0f;
        acc_time += dt;
        if (acc_time >= 1000) {
            vinfo("FPS: %zu", (size_t) floor(1000.0f/dt));
            acc_time = 0;
        }
        start_time = end_time;
    }

    info("Exiting.");
    cleanup_and_exit(0);
}
