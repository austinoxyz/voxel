#ifndef COMMON_H
#define COMMON_H

#include "macros.h"
#include "glfw.h"
#include "shader.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#define info(fmt) \
    __log_internal(__FUNCTION__, __FILE__, __LINE__, 0, "%s", fmt)

#define warn(fmt) \
    __log_internal(__FUNCTION__, __FILE__, __LINE__, 1, "%s", fmt)

#define err(fmt) \
    __log_internal(__FUNCTION__, __FILE__, __LINE__, 2, "%s", fmt);

#define vinfo(fmt, ...) \
    __log_internal(__FUNCTION__, __FILE__, __LINE__, 0, fmt, __VA_ARGS__)

#define vwarn(fmt, ...) \
    __log_internal(__FUNCTION__, __FILE__, __LINE__, 1, fmt, __VA_ARGS__)

#define verr(fmt, ...) \
    __log_internal(__FUNCTION__, __FILE__, __LINE__, 2, fmt, __VA_ARGS__);

void cleanup_and_exit(int exitcode);

void set_cleanup_function(void (*cleanup)(void));

void __log_internal(const char *func, const char *file, int line, int level, const char *fmt, ...);

bool is_debug_mode(void);

void set_debug_mode(bool value);

char *get_filename_base(const char *s); // must free

int file_exists(const char *path);

int directory_exists(const char *path);

char *slurp_file(const char *fpath); // must free

int distance_until_next(const char *s, char c);

long get_time_us(void);

bool is_ascii_whitespace(char c);

double map_range(double value, double in_min, double in_max, double out_min, double out_max);

float clamp(float x, float low, float high);

#ifndef DA_INIT_CAP
#define DA_INIT_CAP (256)
#endif

#define da_init(da)                                                              \
    do {                                                                         \
        (da)->count = 0;                                                         \
        (da)->capacity = DA_INIT_CAP;                                            \
        (da)->items = NULL;                                                      \
        (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
        assert((da)->items != NULL && "Buy more RAM lol");                       \
    } while (0)

#define da_append(da, item)                                                            \
    do {                                                                               \
        if ((da)->count >= (da)->capacity) {                                           \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;     \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));   \
            assert((da)->items != NULL && "Buy more RAM lol");                         \
        }                                                                              \
                                                                                       \
        (da)->items[(da)->count++] = (item);                                           \
    } while (0)

#define da_free(da) free((da).items)

#define da_append_many(da, new_items, new_items_count)                                          \
    do {                                                                                        \
        if ((da)->count + (new_items_count) > (da)->capacity) {                                 \
            if ((da)->capacity == 0) {                                                          \
                (da)->capacity = DA_INIT_CAP;                                                   \
            }                                                                                   \
            while ((da)->count + (new_items_count) > (da)->capacity) {                          \
                (da)->capacity *= 2;                                                            \
            }                                                                                   \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));            \
            assert((da)->items != NULL && "Buy more RAM lol");                                  \
        }                                                                                       \
        memcpy((da)->items + (da)->count, (new_items), (new_items_count)*sizeof(*(da)->items)); \
        (da)->count += (new_items_count);                                                       \
    } while (0)

#define da_resize(da, new_size)                                                        \
    do {                                                                               \
        if ((new_size) > (da)->capacity) {                                             \
            (da)->capacity = (new_size);                                               \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Buy more RAM lol");                         \
        }                                                                              \
        (da)->count = (new_size);                                                      \
    } while (0)
#endif

