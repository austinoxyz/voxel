#include <stdio.h>

#include "common.h"
#include "window.h"
#include "camera.h"
#include "input.h"
#include "world.h"

const ivec2s s_winsize = (ivec2s){ .x=800, .y=600 };
const float  s_aspect = (float) s_winsize.x / (float) s_winsize.y;
const char *s_window_title = "opengl-voxel";

static World s_world;

void cleanup()
{
    world_deinit(&s_world);
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

    if (0 > world_init(&s_world)) {
        err("Failed to initialize world.");
        cleanup_and_exit(2);
    }

    info("Initialized successfully.");
}

void update(float dt)
{
    input_handler_poll();

    camera_update(dt);
    world_update(&s_world, dt);
}

void render(float dt)
{
    glClearColor(0.2, 0.2, 0.2, 0.8);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    world_render(&s_world, dt);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
