#ifndef DRAW_TEXT_H
#define DRAW_TEXT_H

#include "common.h"
#include "glfw.h"

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct GlyphMetric {
    float advance_x, advance_y;
    float bearing_x, bearing_y;
    float width, height;
    float texture_x;
} GlyphMetric;

#define GLYPH_ATLAS_GLYPH_PADDING (1.0f)
#define GLYPH_ATLAS_METRICS_CAPACITY (128)
typedef struct GlyphAtlas {
    FT_UInt     width, height;
    GLuint      texture_id;
    GlyphMetric metrics[GLYPH_ATLAS_METRICS_CAPACITY];
    FT_UInt     glyph_indices[GLYPH_ATLAS_METRICS_CAPACITY];
} GlyphAtlas;

typedef struct Font {
    const char*    filepath;
    size_t         loaded_size;
    short          ascent, descent;
    unsigned short units_per_em;
    GlyphAtlas     atlas;
    FT_Face        face;
} Font;

typedef struct TextVertex {
    vec2s pos;
    vec4s color;
    vec2s uv;
} TextVertex;

typedef struct TextRenderer {
    GLuint vao, vbo, ebo;
    Shader shader;
    struct {
        TextVertex *items;
        size_t count;
        size_t capacity;
    } vertices;
    struct {
        GLuint *items;
        size_t count;
        size_t capacity;
    } indices;
    Font font;
    FT_Library freetype;
} TextRenderer;

typedef enum TextAlignment {
    TEXT_ALIGN_LEFT = 0,
    TEXT_ALIGN_RIGHT,
    TEXT_ALIGN_CENTER
} TextAlignment;

int textrenderer_init(TextRenderer* tr, const char *fontpath);
void textrenderer_deinit(TextRenderer *tr);
void textrenderer_flush(TextRenderer *tr);

vec2s draw_text(TextRenderer *tr, const char *text, size_t textlen, 
                vec2s pos, TextAlignment alignment,
                size_t fontsize, size_t linegap, vec4s color);

vec2s draw_text_centered(TextRenderer *tr, const char *text, size_t textlen,
                         TextAlignment alignment, float x1, float y1, float x2, float y2,
                         size_t fontsize, size_t linegap, vec4s color);

vec2s measure_text(TextRenderer *tr, const char *text, size_t textlen, 
                   size_t fontsize, size_t linegap);

#endif

