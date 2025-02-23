#version 450 core

struct Material {
    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 pos;
    vec3 color;
    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;
};

in vec3 fsPos;
in vec3 fsNormal;
in vec4 fsColor;

out vec4 FragColor;

uniform vec3     viewPos;
uniform Material material;
uniform Light    light;

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(fsNormal);
    vec3 lightDir = normalize(light.pos - fsPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    vec3 viewDir = normalize(viewPos - fsPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular;

    vec3 color = fsColor.rgb * light.color * (specular + ambient + diffuse);
    FragColor = vec4(color, 1.0);
}
