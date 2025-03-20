#ifndef NOISE_H
#define NOISE_H

#define DEFAULT_SEED ((int64_t) 42069)

void noise_init(int64_t seed);
void noise_deinit(void);

double perlin_noise2d(double x, double y);
float simplex_noise2d(float x, float y);
double fbm(double x, double y, unsigned int n_octaves);

#endif
