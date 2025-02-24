#include <stdio.h>

#include "common.h"
#include "window.h"
#include "camera.h"
#include "input.h"
#include "world.h"
#include "player.h"
#include "drawtext.h"

const ivec2s s_winsize = (ivec2s){ .x=1080, .y=720 };
const float  s_aspect = (float) s_winsize.x / (float) s_winsize.y;
const char *s_window_title = "opengl-voxel";

static World  s_world;
static Player s_player;

static TextRenderer s_textrenderer;

static const vec4s s_color_white = (vec4s){ .r=1.0f, .g=1.0f, .b=1.0f, .a=1.0f };

void draw_debug_info();

void cleanup()
{
    world_deinit(&s_world);
    player_deinit(&s_player);
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

    if (0 > textrenderer_init(&s_textrenderer, "font/Iosevka.ttf")) {
        err("Failed to initialize text renderer.");
        cleanup_and_exit(4);
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
    player_update(&s_player, dt);
    world_update(&s_world, dt);
}

void render(float dt)
{
    glClearColor(0.2, 0.2, 0.2, 0.8);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    player_render(&s_player, dt);
    world_render(&s_world, dt);

    draw_debug_info();

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
        handle_input(dt);
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

void draw_debug_info(void)
{
    static const size_t debug_text_buff_sz = 128;
    char player_pos_text[debug_text_buff_sz];
    snprintf(player_pos_text, debug_text_buff_sz,
             "player pos: (%0.3f,%0.3f,%0.3f)", 
             s_player.pos[0], s_player.pos[1], s_player.pos[2]);

    draw_text(&s_textrenderer, player_pos_text, strlen(player_pos_text),
              (vec2s){ .x=100, .y=s_winsize.y-100 }, TEXT_ALIGN_LEFT, 
              32, 0, s_color_white);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    textrenderer_flush(&s_textrenderer);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

