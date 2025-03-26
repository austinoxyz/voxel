#ifndef NOISE_H
#define NOISE_H

#include "FastNoiseLite.h"

#define DEFAULT_SEED ((int64_t) 42069)

void noise_init(int64_t seed);
void noise_deinit(void);

void noise_set_type(fnl_noise_type type);

float noise2d(float x, float y);

#endif
