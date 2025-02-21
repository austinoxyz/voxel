#version 450 core

in vec3 fsPos;
in vec3 fsNormal;
in vec4 fsColor;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    float ambientStrength = 0.13;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(fsNormal);
    vec3 lightDir = normalize(lightPos - fsPos);
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    vec3 color = fsColor.rgb * (ambient + diffuse);
    FragColor = vec4(color, 1.0);
}
