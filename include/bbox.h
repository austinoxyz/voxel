#ifndef BBOX_H
#define BBOX_H

#include "glfw.h"
#include "common.h"

typedef struct Bbox {
    vec3 min, max;
} Bbox;

Bbox make_bbox(float minx, float miny, float minz, float maxx, float maxy, float maxz);

#endif
