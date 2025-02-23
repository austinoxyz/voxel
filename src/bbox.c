#include "bbox.h"

Bbox make_bbox(float minx, float miny, float minz, float maxx, float maxy, float maxz)
{
    return (Bbox) { { minx, miny, minz }, { maxx, maxy, maxz } };
}
