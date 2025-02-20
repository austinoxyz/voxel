#ifndef GLFW_H
#define GLFW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>

#include <stdbool.h>

void glfw_init(void);
void glfw_close(void);

bool glfw_is_initialized(void);

#define gl_check_error() \
    gl_check_error_(__FILE__, __LINE__) 
GLenum gl_check_error_(const char *file, int line);

void gl_debug_message_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                     GLsizei length, const char *message, const void *userParam);

#endif
