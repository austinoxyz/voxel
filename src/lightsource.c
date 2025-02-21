#include "lightsource.h"
#include "camera.h"

static LightSource s_lightsource;
static bool s_lightsource_initialized = false;

void lightsource_compute_sphere(vec3s color);

LightSource* lightsource_get()
{
    assert(s_lightsource_initialized);
    return &s_lightsource;
}

int lightsource_init(float radius, vec3s pos, vec3s color)
{
    assert(!s_lightsource_initialized);

    s_lightsource.radius = radius;
    s_lightsource.pos    = pos;
    s_lightsource.color  = color;

    if (0 > shader_create(&s_lightsource.shader, "src/glsl/light_vs.glsl", "src/glsl/light_fs.glsl")) {
        err("Failed to create light source shader.");
        return -1;
    }

    glUseProgram(s_lightsource.shader);

    mat4 model;
    glm_translate_make(model, &s_lightsource.pos.raw[0]);
    shader_set_uniform_mat4(s_lightsource.shader, "model", model);

    da_init(&s_lightsource.vertices);
    da_init(&s_lightsource.indices);

    lightsource_compute_sphere(color);

    glGenVertexArrays(1, &s_lightsource.vao);
    glGenBuffers(1, &s_lightsource.vbo);
    glGenBuffers(1, &s_lightsource.ebo);

    glBindVertexArray(s_lightsource.vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_lightsource.vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(LightSourceVertex) * s_lightsource.vertices.count, 
                 s_lightsource.vertices.items,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_lightsource.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(GLuint) * s_lightsource.indices.count, 
                 s_lightsource.indices.items,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(LightSourceVertex),
                          (GLvoid *) offsetof(LightSourceVertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(LightSourceVertex),
                          (GLvoid *) offsetof(LightSourceVertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                          sizeof(LightSourceVertex),
                          (GLvoid *) offsetof(LightSourceVertex, color));
    glEnableVertexAttribArray(0);

    /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    s_lightsource_initialized = true;
    return 0;
}

void lightsource_deinit(void)
{
    assert(s_lightsource_initialized);

    glDeleteBuffers(1, &s_lightsource.ebo);
    glDeleteBuffers(1, &s_lightsource.vbo);
    glDeleteVertexArrays(1, &s_lightsource.vao);
    glDeleteProgram(s_lightsource.shader);

    da_free(s_lightsource.vertices);
    da_free(s_lightsource.indices);

    s_lightsource_initialized = false;
}

void lightsource_update(float dt)
{
    assert(s_lightsource_initialized);

    camera_set_shader_projection_and_view(s_lightsource.shader);
}

void lightsource_render(void)
{
    assert(s_lightsource_initialized);

    glUseProgram(s_lightsource.shader);
    glBindVertexArray(s_lightsource.vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_lightsource.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_lightsource.ebo);
    glDrawElements(GL_TRIANGLE_STRIP, 
                   s_lightsource.indices.count, 
                   GL_UNSIGNED_INT, 
                   0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void lightsource_compute_sphere(vec3s color) {
    static const float PI = 3.1415927f;

    static const int sector_count = 36;
    static const int stack_count = 18;
    float x, y, z, xy;
    float nx, ny, nz, length_inv = 1.0f / s_lightsource.radius;
    float sector_step = 2 * PI / sector_count;
    float stack_step = PI / stack_count;
    float sector_angle, stack_angle;

    // create vertices
    for (int i = 0; i <= stack_count; ++i) {
        stack_angle = PI / 2 - i * stack_step;
        xy = s_lightsource.radius * cos(stack_angle);
        z = s_lightsource.radius * sin(stack_angle);
        for (int j = 0; j <= sector_count; ++j) {
            sector_angle = j * sector_step;
            x = xy * cos(sector_angle);
            y = xy * sin(sector_angle);
            nx = x * length_inv;
            ny = y * length_inv;
            nz = z * length_inv;
            const LightSourceVertex v = (LightSourceVertex) {
                { x, y, z }, { nx, ny, nz }, { color.r, color.g, color.b, 1 }
            };
            da_append(&s_lightsource.vertices, v);
        }
    }

    // create indices
    int k1, k2;
    for (int i = 0; i < stack_count; ++i) {
        k1 = i * (sector_count + 1);
        k2 = k1 + sector_count + 1;
        for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
            if (i != 0) {
                da_append(&s_lightsource.indices, k1);
                da_append(&s_lightsource.indices, k2);
                da_append(&s_lightsource.indices, k1 + 1);
            }
            if (i != (stack_count-1)) {
                da_append(&s_lightsource.indices, k1 + 1);
                da_append(&s_lightsource.indices, k2);
                da_append(&s_lightsource.indices, k2 + 1);
            }
        }
    }
}
