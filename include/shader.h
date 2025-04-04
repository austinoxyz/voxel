#ifndef SHADER_H
#define SHADER_H

#include "glfw.h"

typedef GLuint Shader;

int  shader_create(Shader *shader, const char *vspath, const char *fspath);
void shader_destroy(Shader shader);

void shader_set_uniform_int(Shader shader, const char *name, int value);
void shader_set_uniform_float(Shader shader, const char *name, float value);
void shader_set_uniform_mat4s(Shader shader, const char *name, mat4s value);
void shader_set_uniform_mat4(Shader shader, const char *name, mat4 value);
void shader_set_uniform_vec2(Shader shader, const char *name, vec2 value);
void shader_set_uniform_vec2s(Shader shader, const char *name, vec2s value);
void shader_set_uniform_ivec2(Shader shader, const char *name, ivec2 value);
void shader_set_uniform_ivec2s(Shader shader, const char *name, ivec2s value);
void shader_set_uniform_vec3(Shader shader, const char *name, vec3 value);
void shader_set_uniform_vec3s(Shader shader, const char *name, vec3s value);
void shader_set_uniform_vec4s(Shader shader, const char *name, vec4s value);
void shader_set_uniform_bool(Shader shader, const char *name, bool value);

#endif
