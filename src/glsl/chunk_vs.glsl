#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;

out vec3 fsPos;
out vec3 fsNormal;
out vec4 fsColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fsPos    = vec3(model * vec4(inPos, 1.0));
    fsNormal = inNormal;
    fsColor  = inColor;

    gl_Position = projection * view * model * vec4(inPos, 1.0);
}

