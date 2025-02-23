#include "common.h"
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#define SHOW_LOG_LOCATION_INFO (false)

static bool s_debug_mode = false;

void (*s_cleanup)(void) = NULL;

bool is_debug_mode(void)
{
    return s_debug_mode;
}

void set_debug_mode(bool value)
{
    s_debug_mode = value;
}

void set_cleanup_function(void (*cleanup)(void))
{
    assert(!s_cleanup);
    s_cleanup = cleanup;
}

void cleanup_and_exit(int exitcode)
{
    if (!s_cleanup) {
        warn("Cleanup function is not set!");
        return;
    }
    s_cleanup();
    exit(exitcode);
}

void __log_internal(const char *func, const char *file, int line, int level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    switch (level) {
    case 0: printf("[INFO]: "); break;
    case 1: printf("[WARN]: "); break;
    case 2: printf("[ERR]:  "); break;
    default: assert(false && "invalid log level");
    }
    if (SHOW_LOG_LOCATION_INFO) {
        printf(" (in '%s' at %s:%d): ", func, file, line);
    }
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

int file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int directory_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

char *get_filename_base(const char *s)
{
    size_t len = strlen(s);
    char *pos, *base_start = (char*)s;
    while ((pos = memchr(base_start, '/', (s + len) - base_start))) {
        base_start = pos + 1;
    }
    char *ext_pos = memchr(s, '.', len);
    if (!ext_pos) { return base_start; }
    char *ret = NULL;
    if (!(ret = (char*)malloc(sizeof(char)*(ext_pos-base_start)+1))) { return NULL; }
    strncpy(ret, base_start, ext_pos - base_start);
    ret[ext_pos - base_start] = 0;
    return ret;
}

char *slurp_file(const char *fpath)
{
    FILE *file = NULL;
    if (!(file = fopen(fpath, "rb"))) {
        fprintf(stderr, "Failed to open file '%s'.", fpath);
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Failed to seek file");
        fclose(file);
        return NULL;
    }

    size_t file_size = ftell(file);
    rewind(file);

    char *buffer = NULL;
    if (!(buffer = malloc(file_size + 1))) {
        fprintf(stderr, "Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, file_size, file);
    if (read_size != file_size) {
        fprintf(stderr, "Failed to read file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';
    fclose(file);
    return buffer;
}

int distance_until_next(const char *s, char c)
{
    size_t len = strlen(s);
    int i = 0;
    while ((size_t)i < len && s[i] != c) ++i;
    return (size_t)i == len ? -1 : i;
}

long get_time_us(void)
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return 1000000*time.tv_sec + time.tv_nsec/1000;
}

bool is_ascii_whitespace(char c)
{
    return c >= 9 && c <= 13;
}

double map_range(double value, double in_min, double in_max, double out_min, double out_max)
{
    return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
}

float clamp(float x, float low, float high)
{
    if (x < low) {
        return low;
    } else if (x > high) {
        return high;
    } else {
        return x;
    }
}

