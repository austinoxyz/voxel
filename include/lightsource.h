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
    list(LightSourceVertex, vertices);
    list(GLuint, indices);
    World *world;
} LightSource;

int lightsource_init(LightSource *lightsource, float radius, vec3s pos, vec3s color, World *world);
void lightsource_deinit(LightSource *lightsource);
void lightsource_update(LightSource *lightsource, float dt);
void lightsource_render(LightSource *lightsource, float dt);

#endif
