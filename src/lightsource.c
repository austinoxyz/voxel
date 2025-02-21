#include "lightsource.h"
#include "camera.h"

void lightsource_compute_sphere(LightSource *lightsource, vec3s color);

int lightsource_init(LightSource *lightsource, float radius, vec3s pos, vec3s color)
{
    assert(lightsource);

    lightsource->radius = radius;
    lightsource->pos    = pos;
    lightsource->color  = color;

    if (0 > shader_create(&lightsource->shader, "src/glsl/light_vs.glsl", "src/glsl/light_fs.glsl")) {
        err("Failed to create light source shader.");
        return -1;
    }

    glUseProgram(lightsource->shader);

    mat4 model;
    glm_translate_make(model, &lightsource->pos.raw[0]);
    shader_set_uniform_mat4(lightsource->shader, "model", model);

    da_init(&lightsource->vertices);
    da_init(&lightsource->indices);

    lightsource_compute_sphere(lightsource, color);

    glGenVertexArrays(1, &lightsource->vao);
    glGenBuffers(1, &lightsource->vbo);
    glGenBuffers(1, &lightsource->ebo);

    glBindVertexArray(lightsource->vao);
    glBindBuffer(GL_ARRAY_BUFFER, lightsource->vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(LightSourceVertex) * lightsource->vertices.count, 
                 lightsource->vertices.items,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightsource->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(GLuint) * lightsource->indices.count, 
                 lightsource->indices.items,
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return 0;
}

void lightsource_deinit(LightSource *lightsource)
{
    assert(lightsource);

    glDeleteBuffers(1, &lightsource->ebo);
    glDeleteBuffers(1, &lightsource->vbo);
    glDeleteVertexArrays(1, &lightsource->vao);
    glDeleteProgram(lightsource->shader);

    da_free(lightsource->vertices);
    da_free(lightsource->indices);

}

void lightsource_update(LightSource *lightsource, float dt)
{
    assert(lightsource);

    camera_set_shader_projection_and_view(lightsource->shader);
}

void lightsource_render(LightSource *lightsource, float dt)
{
    assert(lightsource);

    glUseProgram(lightsource->shader);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(lightsource->vao);
    glBindBuffer(GL_ARRAY_BUFFER, lightsource->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightsource->ebo);
    glDrawElements(GL_TRIANGLE_STRIP, 
                   lightsource->indices.count, 
                   GL_UNSIGNED_INT, 
                   0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
    glUseProgram(0);
}

void lightsource_compute_sphere(LightSource *lightsource, vec3s color) 
{
    assert(lightsource);

    // increase to 36,18 for higher resolution sphere
    static const int sector_count = 16;
    static const int stack_count = 8;

    float x, y, z, xy;
    float nx, ny, nz, length_inv = 1.0f / lightsource->radius;
    float sector_step = 2 * PI / sector_count;
    float stack_step = PI / stack_count;
    float sector_angle, stack_angle;

    // create vertices
    for (int i = 0; i <= stack_count; ++i) {
        stack_angle = PI / 2 - i * stack_step;
        xy = lightsource->radius * cos(stack_angle);
        z = lightsource->radius * sin(stack_angle);
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
            da_append(&lightsource->vertices, v);
        }
    }

    // create indices
    int k1, k2;
    for (int i = 0; i < stack_count; ++i) {
        k1 = i * (sector_count + 1);
        k2 = k1 + sector_count + 1;
        for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
            if (i != 0) {
                da_append(&lightsource->indices, k1);
                da_append(&lightsource->indices, k2);
                da_append(&lightsource->indices, k1 + 1);
            }
            if (i != (stack_count-1)) {
                da_append(&lightsource->indices, k1 + 1);
                da_append(&lightsource->indices, k2);
                da_append(&lightsource->indices, k2 + 1);
            }
        }
    }
}
