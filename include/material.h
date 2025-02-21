#ifndef MATERIAL_H
#define MATERIAL_H

#include "glfw.h"
#include "common.h"

typedef struct Material {
    vec3 ambient, diffuse, spectral;
    float spectral_intensity;
} Material;

#endif
