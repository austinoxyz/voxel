#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;

flat out vec2 fsPos;
out vec4 fsColor;
out vec2 fsUv;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position, 1.0, 1.0);
    fsPos = gl_Position.xy;
    fsColor = color;
    fsUv = uv;
}

