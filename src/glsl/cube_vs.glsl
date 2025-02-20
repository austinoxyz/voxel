#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 inFragColor;

void main()
{
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    inFragColor = inColor;
}

