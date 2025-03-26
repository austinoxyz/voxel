#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "common.h"

#include "noise.h"

fnl_state s_noise_ctx;
bool      s_noise_ctx_initialized = false;
int64_t   s_seed = DEFAULT_SEED;

void noise_init(int64_t seed)
{
    s_seed = seed;
    s_noise_ctx = fnlCreateState();
    s_noise_ctx.seed = s_seed;
    s_noise_ctx.noise_type = FNL_NOISE_VALUE_CUBIC;
    s_noise_ctx_initialized = true;
}

void noise_deinit(void)
{
    if (!s_noise_ctx_initialized) return;
    s_noise_ctx_initialized = false;
}

void noise_set_type(fnl_noise_type type)
{
    s_noise_ctx.noise_type = type;
}

float noise2d(float x, float y)
{
    float n = fnlGetNoise2D(&s_noise_ctx, x, y);
    return (n+1)*0.5;
}
