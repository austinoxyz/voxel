#include <stdio.h>

#include "common.h"
#include "window.h"
#include "camera.h"
#include "input.h"
#include "world.h"
#include "player.h"

#define DEBUG (0)

const ivec2s s_winsize = (ivec2s){ .x=1080, .y=720 };
const float  s_aspect = (float) s_winsize.x / (float) s_winsize.y;
const char *s_window_title = "opengl-voxel";

static World  s_world;
static Player s_player;

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

    if (0 > window_create(s_winsize, s_window_title, 60)) {
        err("Failed to create window.");
        cleanup_and_exit(1);
    }

    input_handler_init();

    if (0 > world_init(&s_world, &s_player)) {
        err("Failed to initialize world.");
        cleanup_and_exit(2);
    }

    if (0 > player_init(&s_player, &s_world)) {
        err("Failed to initialize player.");
        cleanup_and_exit(3);
    }

    info("Initialized successfully.");
}

void handle_input(float dt)
{
    input_handler_poll();
    player_handle_input(&s_player, dt);
}

void update(float dt)
{
    input_handler_poll();
    player_handle_input(&s_player, dt);

    player_update(&s_player, dt);
    world_update(&s_world, dt);
}

void render(float dt)
{
    glClearColor(0.2, 0.2, 0.2, 0.8);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if defined(DEBUG) && DEBUG == 1
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    player_render(&s_player, dt);
    world_render(&s_world, dt);

#if defined(DEBUG) && DEBUG == 1
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    glfwSwapBuffers(window_get_handle());
    glfwPollEvents();
}

int main(void) 
{
    init();

    long acc_time, end_time, start_time, dt;
    end_time = start_time = get_time_us();
    acc_time = dt = 0;

    /* long prev_time = get_time_us(); */
    /* long curr_time, dt; */
    /* long acc_time = 0; */
    /* float lag; */

    while (!glfwWindowShouldClose(window_get_handle())) 
    {
        /* curr_time = get_time_us(); */
        /* dt = curr_time - prev_time; */
        /* prev_time = curr_time; */
        /* lag += dt/1000; */
        /* acc_time += dt; */

        /* if (acc_time >= 1000000) { */
        /*     vinfo("FPS: %zu", (size_t) floor(1000000.0f/dt)); */
        /*     acc_time = 0; */
        /* } */

        /* handle_input(dt); */

        /* while (lag >= window_get()->ms_per_update) { */
        /*     update(dt); */
        /*     lag -= window_get()->ms_per_update; */
        /* } */

        /* render(dt); */

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
