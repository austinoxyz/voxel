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

int seek_for_char(const char *s, int start, char c);

char *slurp_file(const char *fpath); // must free

int distance_until_next(const char *s, char c);

long get_time_us(void);

bool is_ascii_whitespace(char c);

double map_range(double value, double in_min, double in_max, double out_min, double out_max);

double clamp(double x, double low, double high);

double fade(double t);

double lerp(double x1, double x2, double t);

#endif
