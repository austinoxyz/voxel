// thank you to https://rtouti.github.io/graphics/perlin-noise-algorithm

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "common.h"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#include "noise.h"

#include "open-simplex-noise.h"

#define NOISE_P_SIZE (256)
int p[2*NOISE_P_SIZE];

int64_t s_seed = DEFAULT_SEED;

// for open-simplex-noise-in-c
struct osn_context *s_osn_ctx = NULL;

void noise_deinit(void)
{
    if (s_osn_ctx) {
        open_simplex_noise_free(s_osn_ctx);
    }
}

void noise_init(int64_t seed)
{
    s_seed = seed;
    srand(s_seed);
    open_simplex_noise(s_seed, &s_osn_ctx);
}

float simplex_noise2d(float x, float y)
{
    float n = open_simplex_noise2(s_osn_ctx, x, y);
    return (n+1)*0.5;
}

void shuffle_p(void) 
{
    for (int i = 0; i < NOISE_P_SIZE-1; ++i) {
        int i2 = rand() % (2 * NOISE_P_SIZE);
        if (i2 < NOISE_P_SIZE) {
            int tmp = p[i];
            p[i] = p[i2];
            p[i2] = tmp;
        }
    }
}

void make_permutation_p(void) 
{
    for (int i = 0; i < NOISE_P_SIZE; ++i) {
        p[i] = i;
    }
    shuffle_p();
    for (int i = 0; i < NOISE_P_SIZE; ++i) {
        p[i+NOISE_P_SIZE] = p[i];
    }
}

vec2s corner_vec2(int v) 
{
    switch (v & 3) {
    case 0: return (vec2s){ .x= 1.0, .y= 1.0 };
    case 1: return (vec2s){ .x=-1.0, .y= 1.0 };
    case 2: return (vec2s){ .x=-1.0, .y=-1.0 };
    case 3: return (vec2s){ .x= 1.0, .y=-1.0 };
    }
    __builtin_unreachable();
}

#define NOISE_FREQ (0.01f)

double perlin_noise2d_impl(double, double);

double fbm(double x, double y, unsigned int n_octaves) 
{
    double result = 0.0;
    double ampl = 1.0;
    double freq = 0.005;
    /* double freq = 1.0; */

    for (unsigned i = 0; i < n_octaves; ++i) 
    {
        /* double n = ampl * perlin_noise2d(0.05*freq*(x+s_seed), 0.05*freq*(y+s_seed)); */

        double n = ampl * simplex_noise2d(0.05*freq*(x+s_seed), 0.05*freq*(y+s_seed));

        result += n;
        ampl *= 0.5;
        freq *= 2.0;
    }

    return result / 2.25;
}

double perlin_noise2d(double x, double y)
{
    double n = perlin_noise2d_impl(x, y);
    /* printf("n = %lf\n", n); */
    return (1.0+n)*0.5;
}

// perlin noise
double perlin_noise2d_impl(double _x, double _y) 
{
    int x = (int) floorf(_x) & 0xff;
    int y = (int) floorf(_y) & 0xff;
    double xf = _x - floorf(_x);
    double yf = _y - floorf(_y);

    vec2s top_right = (vec2s){ .x=xf-1.0, .y=yf-1.0 };
    vec2s top_left  = (vec2s){ .x=xf,     .y=yf-1.0 };
    vec2s bot_right = (vec2s){ .x=xf-1.0, .y=yf };
    vec2s bot_left  = (vec2s){ .x=xf,     .y=yf };

    vec2s corner_top_right = corner_vec2(p[p[x+1]+y+1]);
    vec2s corner_top_left  = corner_vec2(p[p[x]+y+1]);
    vec2s corner_bot_right = corner_vec2(p[p[x+1]+y]);
    vec2s corner_bot_left  = corner_vec2(p[p[x]+y]);

    double dot_top_right = glms_vec2_dot(top_right, corner_top_right);
    double dot_top_left  = glms_vec2_dot(top_left, corner_top_left);
    double dot_bot_right = glms_vec2_dot(bot_right, corner_bot_right);
    double dot_bot_left = glms_vec2_dot(bot_left , corner_bot_right);

    double u = fade(xf);
    double v = fade(yf);

    return lerp(lerp(dot_bot_left,  dot_top_left,  v),
                lerp(dot_bot_right, dot_top_right, v),
                u);

}

void set_seed(long seed)
{
    s_seed = seed;
    noise_deinit();
    noise_init(s_seed);
}
