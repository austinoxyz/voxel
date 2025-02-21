#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "glfw.h"
#include "common.h"

typedef struct LightSourceVertex {
    vec3 pos;
    vec3 normal;
    vec4 color;
} LightSourceVertex;

typedef struct World World;

typedef struct LightSource {
    GLuint vao, vbo, ebo;
    Shader shader;
    vec3s pos;
    vec3s color;
    float radius;
    struct {
        LightSourceVertex *items;
        size_t count;
        size_t capacity;
    } vertices;
    struct {
        GLuint *items;
        size_t count;
        size_t capacity;
    } indices;
    World *world;
} LightSource;

int lightsource_init(LightSource *lightsource, float radius, vec3s pos, vec3s color, World *world);
void lightsource_deinit(LightSource *lightsource);
void lightsource_update(LightSource *lightsource, float dt);
void lightsource_render(LightSource *lightsource, float dt);

#endif
