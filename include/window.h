#ifndef WINDOW_H
#define WINDOW_H

#include "glfw.h"

typedef struct Window {
    GLFWwindow *handle;
    ivec2s      size;
    const char *title;
    float       aspect;
    int         target_fps;
    float       ms_per_update;
} Window;

int         window_create(ivec2s size, const char *title, int fps);
void        window_destroy(void);
Window     *window_get(void);
GLFWwindow *window_get_handle(void);
bool        window_is_initialized(void);

#endif
