#version 450 core

in vec4 inFragColor;
out vec4 outFragColor;

uniform vec3 lightColor;

void main()
{
    float ambientStrength = 0.35;
    vec3 ambient = ambientStrength * lightColor;
    outFragColor = vec4(ambient * inFragColor.rgb, 1.0);
}
