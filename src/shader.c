#include "shader.h"
#include "common.h"

#include <assert.h>
#include <string.h>

int shader_compile(GLuint *id, GLenum type, const char *srcpath);
int shader_link(GLuint *id, GLuint vsid, GLuint fsid);

int shader_create(Shader *shader, const char *vspath, const char *fspath)
{
    assert(shader);

    GLuint id, vsid, fsid; 

    if (0 > shader_compile(&vsid, GL_VERTEX_SHADER, vspath)) {
        return -1;
    }

    if (0 > shader_compile(&fsid, GL_FRAGMENT_SHADER, fspath)) {
        return -2;
    }

    if (0 > shader_link(&id, vsid, fsid)) {
        return -3;
    }

    glDeleteShader(vsid);
    glDeleteShader(fsid);

    *shader = id;
    return 0;
}

void shader_destroy(Shader shader)
{
    glDeleteProgram(shader);
}

int shader_compile(GLuint *id, GLenum type, const char *srcpath)
{
    assert(id);

    char *src = NULL;
    if (!(src = slurp_file((srcpath)))) {
        fprintf(stderr, "Failed to slurp file at '%s'.\n", srcpath);
        return -1;
    }

    GLuint id_ = glCreateShader((type));
    glShaderSource(id_, 1, (const GLchar* const*)&src, NULL);
    glCompileShader(id_);

    GLint success = 0;
    GLchar infomsg[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id_, 512, NULL, infomsg);
        verr("Failed to compile shader: %s", infomsg);
        return -2;
    }
    vinfo("Compiled shader '%s'", srcpath);

    gl_check_error();

    free(src);

    *id = id_;
    return 0;
}

int shader_link(GLuint *id, GLuint vsid, GLuint fsid)
{
    assert(id);

    GLuint id_ = glCreateProgram();
    glAttachShader(id_, vsid);
    glAttachShader(id_, fsid);
    glLinkProgram(id_);

    GLint success = 0;
    GLchar infomsg[512];
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id_, 512, NULL, infomsg);
        fprintf(stderr, "Failed to link shader: %s\n", infomsg);
        return -1;
    }

    *id = id_;
    return 0;
}

GLint get_uniform_location(Shader shader, const char *name) 
{
    GLint location;
    if (-1 == (location = glGetUniformLocation((shader), (name)))) {
        fprintf(stderr, "Invalid uniform name: '%s'", (name));
        exit(1);
    }
    return location;
}

void shader_set_uniform_float(Shader shader, const char *name, float value)
{
    glUseProgram(shader);
    glUniform1f(get_uniform_location(shader, name), value);
    gl_check_error();
}

void shader_set_uniform_int(Shader shader, const char *name, int value)
{
    glUseProgram(shader);
    glUniform1i(get_uniform_location(shader, name), value);
    gl_check_error();
}

void shader_set_uniform_bool(Shader shader, const char *name, bool value)
{
    glUseProgram(shader);
    shader_set_uniform_int(shader, name, (int) value);
    gl_check_error();
}

void shader_set_uniform_mat4s(Shader shader, const char *name, mat4s value)
{
    glUseProgram(shader);
    glUniformMatrix4fv(get_uniform_location(shader, name), 
                       1, 
                       GL_FALSE, 
                       (const GLfloat *) value.raw);
    gl_check_error();
}

void shader_set_uniform_mat4(Shader shader, const char *name, mat4 value)
{
    glUseProgram(shader);
    glUniformMatrix4fv(get_uniform_location(shader, name),
                       1,
                       GL_FALSE,
                       (const GLfloat *) value);
    gl_check_error();
}

void shader_set_uniform_vec3(Shader shader, const char *name, vec3 value)
{
    glUseProgram(shader);
    glUniform3fv(get_uniform_location(shader, name), 
                 1, 
                 (const GLfloat *)value);
    gl_check_error();
}

void shader_set_uniform_vec3s(Shader shader, const char *name, vec3s value)
{
    glUseProgram(shader);
    glUniform3fv(get_uniform_location(shader, name), 
                 1, 
                 (const GLfloat *)value.raw);
    gl_check_error();
}

void shader_set_uniform_vec4s(Shader shader, const char *name, vec4s value)
{
    glUseProgram(shader);
    glUniform4fv(get_uniform_location(shader, name), 
                 1, 
                 (const GLfloat *)value.raw);
    gl_check_error();
}
