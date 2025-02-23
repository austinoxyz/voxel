#include "window.h"
#include "macros.h"
#include "common.h"

#include <assert.h>

static Window s_window;
static bool s_window_is_initialized = false;

void gl_debug_message_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                     GLsizei length, const char *message, const void *userParam);

Window *window_get(void)
{
    assert(s_window_is_initialized);
    return &s_window;
}

GLFWwindow *window_get_handle(void)
{
    assert(s_window_is_initialized);

    return s_window.handle;
}

bool window_is_initialized(void)
{
    return s_window_is_initialized;
}

void gl_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    assert(window);
    s_window.size = (ivec2s){ .x=width, .y=height };
    s_window.aspect = (float) width / (float) height;
    glViewport(0, 0, width, height);
}

int window_create(ivec2s size, const char *title, int fps)
{
    assert(glfw_is_initialized());
    assert(!s_window_is_initialized);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE,     GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE,       GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING,      GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    GLFWwindow *handle = glfwCreateWindow(size.x, size.y, title, NULL, NULL);
    if (!handle) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load glad.\n");
        return -2;
    }

    glViewport(0, 0, size.x, size.y);
    glfwSetFramebufferSizeCallback(handle, gl_framebuffer_size_callback);

    glEnable(GL_CULL_FACE);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_message_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);

    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    { 
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        bool debug_output_enabled = (flags & GL_CONTEXT_FLAG_DEBUG_BIT);

        info("Window created.");
        vinfo("OpenGL debug output enabled: %s", 
              debug_output_enabled ? "true" : "false");
    }

    s_window.handle = handle;
    s_window.size   = size;
    s_window.title  = title;
    s_window.aspect = (float) size.x / (float) size.y;

    s_window.target_fps    = fps;
    s_window.ms_per_update = 1000.0f / fps;

    s_window_is_initialized = true;

    return 0;
}

void window_destroy(void)
{
    assert(s_window_is_initialized);

    glfwDestroyWindow(s_window.handle);

    s_window_is_initialized = false;
}
