#include "cubeshader.h"

#include "common.h"

Shader s_cubeshader;

void cubeshader_init(void)
{
    if (0 > shader_create(&s_cubeshader, "src/glsl/cube_vs.glsl", "src/glsl/cube_fs.glsl")) {
        err("Failed to create cubeshader.");
        cleanup_and_exit(1);
    }
}

Shader cubeshader_get(void)
{
    return s_cubeshader;
}

void cubeshader_deinit(void)
{
    glDeleteProgram(s_cubeshader);
}

