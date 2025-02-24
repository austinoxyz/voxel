#include "drawtext.h"

#include "window.h"
#include "shader.h"

int font_init_glyph_atlas(Font *font);

void draw_character(TextRenderer *tr, char c, vec2s *cursor, size_t fontsize, vec4s color);

int textrenderer_init(TextRenderer *tr, const char *fontpath)
{
    assert(window_is_initialized());
    assert(tr);

    if (FT_Init_FreeType(&tr->freetype)) {
        err("Failed to initialize freetype.");
        return -1;
    }

    if (FT_New_Face(tr->freetype, fontpath, 0, &tr->font.face)) {
        verr("Failed to load font face at '%s'.", fontpath);
        return -3;
    }

    da_init(&tr->vertices);
    da_init(&tr->indices);

    static const int default_fontface_loaded_size = 64;
    FT_Set_Pixel_Sizes(tr->font.face, 0, default_fontface_loaded_size);

    tr->font.filepath     = fontpath;
    tr->font.loaded_size  = default_fontface_loaded_size;
    tr->font.ascent       = tr->font.face->ascender;
    tr->font.descent      = tr->font.face->descender;
    tr->font.units_per_em = tr->font.face->units_per_EM;

    if (0 > shader_create(&tr->shader, "src/glsl/text_vs.glsl", "src/glsl/text_fs.glsl")) {
        err("Failed to create shader for text renderer.");
        return -2;
    }

    gl_check_error();

    glUseProgram(tr->shader);
    glGenVertexArrays(1, &tr->vao);
    glGenBuffers(1, &tr->vbo);
    glGenBuffers(1, &tr->ebo);

    gl_check_error();

    if (0 > font_init_glyph_atlas(&tr->font)) {
        verr("Failed to initialize the glyph atlas for font at '%s'.", fontpath);
        return -4;
    }

    glBindVertexArray(tr->vao);
    glBindBuffer(GL_ARRAY_BUFFER, tr->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tr->ebo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(TextVertex), 
                          (GLvoid *) offsetof(TextVertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 
                          sizeof(TextVertex), 
                          (GLvoid *) offsetof(TextVertex, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(TextVertex), 
                          (GLvoid *) offsetof(TextVertex, uv));
    /* glEnableVertexAttribArray(0); */

    gl_check_error();

    mat4 ortho_projection;
    glm_ortho(0, window_get()->size.x, 0, window_get()->size.y, 0.1, 100.0, ortho_projection);
    shader_set_uniform_mat4(tr->shader, "projection", ortho_projection);

    /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); */
    /* glBindBuffer(GL_ARRAY_BUFFER, 0); */
    /* glBindVertexArray(0); */
    /* glUseProgram(0); */

    info("Initialized text renderer.");
    return 0;
}

void textrenderer_deinit(TextRenderer *tr)
{
    assert(tr);

    FT_Done_Face(tr->font.face);
    glDeleteTextures(1, &tr->font.atlas.texture_id);

    FT_Done_FreeType(tr->freetype);

    da_free(tr->vertices);
    da_free(tr->indices);

    glDeleteBuffers(1, &tr->vbo);
    glDeleteBuffers(1, &tr->ebo);
    glDeleteVertexArrays(1, &tr->vao);
    glDeleteProgram(tr->shader);
}

int font_init_glyph_atlas(Font *font)
{
    assert(font);

    font->atlas.width = 0;
    font->atlas.height = 0;

    for (unsigned char c = 32; c < 128; ++c) {
        if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
            err("Could not load glyph.");
            return -1;
        }
        font->atlas.width += font->face->glyph->bitmap.width + GLYPH_ATLAS_GLYPH_PADDING;
        if (font->atlas.height < font->face->glyph->bitmap.rows) {
            font->atlas.height = font->face->glyph->bitmap.rows;
        }
    }

    glActiveTexture(GL_TEXTURE0); 
    glGenTextures(1, &font->atlas.texture_id);
    glBindTexture(GL_TEXTURE_2D, font->atlas.texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Turn off requirement that pixel size will be a multiple 
    // of 4 bytes - bitmap pixels are 0..255, 1 byte
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLsizei) font->atlas.width,
                 (GLsizei) font->atlas.height, 0, GL_RED, 
                 GL_UNSIGNED_BYTE, NULL);

    int x = 0;
    for (unsigned char c = 32; c < 128; ++c) {
        if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
            verr("Could not load glyph with ascii code %d.", (int) c);
            return -2;
        }

        if (FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_NORMAL)) {
            verr("Could not render glyph with ascii code %d.", (int) c);
            return -3;
        }

        font->atlas.metrics[c].advance_x = font->face->glyph->advance.x >> 6;
        font->atlas.metrics[c].advance_y = font->face->glyph->advance.y >> 6;
        font->atlas.metrics[c].bearing_x = font->face->glyph->bitmap_left;
        font->atlas.metrics[c].bearing_y = font->face->glyph->bitmap_top;
        font->atlas.metrics[c].width     = font->face->glyph->bitmap.width;
        font->atlas.metrics[c].height    = font->face->glyph->bitmap.rows;
        font->atlas.metrics[c].texture_x = (float) x / (float) font->atlas.width;

        font->atlas.glyph_indices[c] = FT_Get_Char_Index(font->face, c);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, font->face->glyph->bitmap.width,
                        font->face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE,
                        font->face->glyph->bitmap.buffer);

        x += font->face->glyph->bitmap.width + GLYPH_ATLAS_GLYPH_PADDING;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return 0;
}

void draw_character(TextRenderer *tr, char c, vec2s *cursor, size_t fontsize, vec4s color) 
{
    const GlyphMetric metrics = tr->font.atlas.metrics[(size_t)c];

    const float scale = (float) fontsize / tr->font.loaded_size;

    const vec2s basepos = (vec2s) {
        .x=(cursor->x + scale*metrics.bearing_x),
        .y=(cursor->y - scale*(metrics.height - metrics.bearing_y))
    };

    const float width  = scale * metrics.width;
    const float height = scale * metrics.height;

    const vec2s p1 = (vec2s) { .x=basepos.x,           .y=basepos.y };
    const vec2s p2 = (vec2s) { .x=basepos.x,           .y=(basepos.y + height) };
    const vec2s p3 = (vec2s) { .x=(basepos.x + width), .y=basepos.y };
    const vec2s p4 = (vec2s) { .x=(basepos.x + width), .y=(basepos.y + height) };

    const float uvwidth  = (float) metrics.width  / tr->font.atlas.width;
    const float uvheight = (float) metrics.height / tr->font.atlas.height;

    const vec2s uv1 = (vec2s) { .x=metrics.texture_x,             .y=uvheight };
    const vec2s uv2 = (vec2s) { .x=metrics.texture_x,             .y=0 };
    const vec2s uv3 = (vec2s) { .x=(metrics.texture_x + uvwidth), .y=uvheight };
    const vec2s uv4 = (vec2s) { .x=(metrics.texture_x + uvwidth), .y=0 };

    const TextVertex v1 = (TextVertex){ p1, color, uv1 };
    const TextVertex v2 = (TextVertex){ p1, color, uv2 };
    const TextVertex v3 = (TextVertex){ p1, color, uv3 };
    const TextVertex v4 = (TextVertex){ p1, color, uv4 };

    vinfo("rendering char='%c':", c);
    vinfo("v1: pos=(%0.2f,%0.2f), uv=(%0.2f,%0.2f), color=(%0.1f,%0.1f,%0.1f,%0.1f)",
          p1.x, p1.y, uv1.x, uv1.y, color.r, color.g, color.b, color.a);
    vinfo("v2: pos=(%0.2f,%0.2f), uv=(%0.2f,%0.2f), color=(%0.1f,%0.1f,%0.1f,%0.1f)",
          p2.x, p2.y, uv2.x, uv2.y, color.r, color.g, color.b, color.a);
    vinfo("v3: pos=(%0.2f,%0.2f), uv=(%0.2f,%0.2f), color=(%0.1f,%0.1f,%0.1f,%0.1f)",
          p3.x, p3.y, uv3.x, uv3.y, color.r, color.g, color.b, color.a);
    vinfo("v4: pos=(%0.2f,%0.2f), uv=(%0.2f,%0.2f), color=(%0.1f,%0.1f,%0.1f,%0.1f)",
          p4.x, p4.y, uv4.x, uv4.y, color.r, color.g, color.b, color.a);

    da_append(&tr->vertices, v1);
    da_append(&tr->vertices, v2);
    da_append(&tr->vertices, v3);
    da_append(&tr->vertices, v4);

    const GLuint start = tr->vertices.count - 4;
    da_append(&tr->indices, (GLuint) 0 + start);
    da_append(&tr->indices, (GLuint) 2 + start);
    da_append(&tr->indices, (GLuint) 1 + start);
    da_append(&tr->indices, (GLuint) 2 + start);
    da_append(&tr->indices, (GLuint) 0 + start);
    da_append(&tr->indices, (GLuint) 3 + start);

    cursor->x += metrics.advance_x * scale;
}

vec2s draw_text(TextRenderer *tr, const char *text, size_t textlen, 
                  vec2s pos, TextAlignment alignment,
                  size_t fontsize, size_t linegap, vec4s color) 
{
    info("-------------------------------------------");

    const vec2s textsize = measure_text(tr, text, textlen, fontsize, linegap);

    const float scale = (float) fontsize / tr->font.units_per_em;

    int next_newline = seek_for_char(text, 0, '\n');
    if (-1 == next_newline) {
        next_newline = textlen;
    }
    float linewidth = measure_text(tr, text, (size_t)next_newline, fontsize, linegap).x;
    float x_offset = 0;
    if (alignment == TEXT_ALIGN_CENTER) {
        x_offset = (textsize.x - linewidth)/2.0f;
    } else if (alignment == TEXT_ALIGN_RIGHT) {
        x_offset = (textsize.x - linewidth);
    }

    vec2s cursor = (vec2s){ .x=pos.x+x_offset, .y=pos.y };

    size_t i = 0;
    while (i < textlen) {
        if (text[i] != '\n') {
            draw_character(tr, text[i], &cursor, fontsize, color);
            i += 1;
            continue;
        }

        if (i == textlen - 1) { break; }

        while (i < textlen && text[i] == '\n') {
            cursor.y -= scale*(tr->font.ascent - tr->font.descent) + linegap;
            i += 1;
        }

        if (i == textlen) { break; }

        next_newline = seek_for_char(text, i, '\n');
        if (-1 == next_newline) {
            next_newline = textlen;
        } 
        linewidth = measure_text(tr, &text[i], (size_t)next_newline - i, fontsize, linegap).x;
        x_offset = 0;
        if (alignment == TEXT_ALIGN_CENTER) {
            x_offset = (textsize.x - linewidth)/2.0f;
        } else if (alignment == TEXT_ALIGN_RIGHT) {
            x_offset = (textsize.x - linewidth);
        }

        cursor.x = pos.x + x_offset;
    }

    return cursor;
}

vec2s draw_text_centered(TextRenderer *tr, const char *text, size_t textlen,
                           TextAlignment alignment, float x1, float y1, float x2, float y2,
                           size_t fontsize, size_t linegap, vec4s color)
{
    const vec2s size = measure_text(tr, text, textlen, fontsize, linegap);

    const vec2s pos = (vec2s) { 
        .x=(x1+((x2-x1)-size.x)/2.f), 
        .y=(y1+((y2-y1)-(((y2-y1)-size.y)/2.f))-((float)fontsize/tr->font.units_per_em)*tr->font.ascent),
    };

    return draw_text(tr, text, textlen, pos, alignment, fontsize, linegap, color);
}

vec2s measure_text(TextRenderer *tr, const char *text, size_t textlen, size_t fontsize, size_t linegap)
{
    const float linescale = (float) fontsize / tr->font.units_per_em;

    vec2s size = (vec2s){ .x=0, .y=(linescale * (tr->font.ascent - tr->font.descent)+linegap) };
    float x = 0;
    for (size_t i = 0; i < textlen; ++i) {
        const char c = text[i];

        if (c == '\n') {
            if (i == textlen - 1) {
                break;
            }
            x = 0;
            size.y += linescale*(tr->font.ascent - tr->font.descent) + linegap;
            continue;
        }

        const float advance = tr->font.atlas.metrics[(size_t)c].advance_x;
        const float fontscale = (float) fontsize / tr->font.loaded_size;
        x += advance * fontscale;

        if (x > size.x) {
            size.x = x;
        }
    }

    size.y -= linescale * tr->font.descent;

    return size;
}

void textrenderer_flush(TextRenderer *tr) 
{
    glUseProgram(tr->shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tr->font.atlas.texture_id);

    glBindVertexArray(tr->vao);

    glBindBuffer(GL_ARRAY_BUFFER, tr->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 tr->vertices.count * sizeof(TextVertex),
                 tr->vertices.items,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tr->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 tr->indices.count * sizeof(GLuint),
                 tr->indices.items,
                 GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, tr->indices.count, GL_UNSIGNED_INT, 0);
    
    gl_check_error();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    tr->vertices.count = 0;
    tr->indices.count = 0;
}

