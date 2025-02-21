#include "chunk.h"
#include "shader.h"
#include "camera.h"
#include "lightsource.h"

static GLuint s_chunk_vao, s_chunk_vbo;
static Shader s_chunk_shader;

void chunk_render_block(Chunk *chunk, ivec3s pos, Block block);
void render_standard_block(Chunk *chunk, ivec3s pos, vec4s color);
void render_dirt_block(Chunk *chunk, ivec3s pos);
void render_stone_block(Chunk *chunk, ivec3s pos);
void render_grass_block(Chunk *chunk, ivec3s pos);

int init_chunk_renderer(void)
{
    if (0 > shader_create(&s_chunk_shader, "src/glsl/chunk_vs.glsl", "src/glsl/chunk_fs.glsl")) {
        fprintf(stderr, "Failed to create chunk shader.\n");
        return -1;
    }

    glUseProgram(s_chunk_shader);

    glGenVertexArrays(1, &s_chunk_vao);
    glGenBuffers(1, &s_chunk_vbo);

    glBindVertexArray(s_chunk_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_chunk_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(BlockVertex),
                          (GLvoid *) offsetof(BlockVertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(BlockVertex),
                          (GLvoid *) offsetof(BlockVertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                          sizeof(BlockVertex),
                          (GLvoid *) offsetof(BlockVertex, color));
    glEnableVertexAttribArray(0);

    mat4 model;
    glm_mat4_identity(model);

    shader_set_uniform_mat4(s_chunk_shader, "model", model);
    shader_set_uniform_vec3s(s_chunk_shader, "lightColor", lightsource_get()->color);
    shader_set_uniform_vec3s(s_chunk_shader, "lightPos",   lightsource_get()->pos);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    return 0;
}

void deinit_chunk_renderer(void)
{
    glDeleteBuffers(1, &s_chunk_vbo);
    glDeleteVertexArrays(1, &s_chunk_vao);
    shader_destroy(s_chunk_shader);
}

int chunk_new(Chunk *chunk, ivec2s pos)
{
    assert(chunk);

    if (0 > chunkmesh_new(&chunk->mesh)) {
        fprintf(stderr, "Failed to initialize chunkmesh.\n");
        return -1;
    }

    Block ***blocks = NULL;
    if (!(blocks = (Block***)malloc(CHUNK_SZ * sizeof(Block**)))) {
        err("Failed to allocate memory for chunk.");
        cleanup_and_exit(1);
    }
    for (int i = 0; i < CHUNK_SZ; ++i) {
        if (!(blocks[i] = (Block**)malloc(CHUNK_SZ * sizeof(Block*)))) {
            err("Failed to allocate memory for chunk.");
            cleanup_and_exit(1);
        }
    }
    for (int i = 0; i < CHUNK_SZ; ++i) {
        for (int j = 0; j < CHUNK_SZ; ++j) {
            if (!(blocks[i][j] = (Block*)malloc(CHUNK_SZ * sizeof(Block)))) {
                err("Failed to allocate memory for chunk.");
                cleanup_and_exit(1);
            }
            for (int k = 0; k < CHUNK_SZ; ++k) {
                blocks[i][j][k] = (Block){ BLOCK_AIR };
            }
        }
    }

    chunk->blocks = blocks;
    chunk->pos = pos;
    chunk->dirty = false;
    return 0;
}

void chunk_delete(Chunk *chunk)
{
    assert(chunk);

    assert(chunk->blocks);
    for (int i = 0; i < CHUNK_SZ; ++i) {
        assert(chunk->blocks[i]);
        for (int j = 0; j < CHUNK_SZ; ++j) {
            assert(chunk->blocks[i][j]);
            free(chunk->blocks[i][j]);
        }
        free(chunk->blocks[i]);
    }
    free(chunk->blocks);
}

void chunk_update(Chunk *chunk, float dt)
{
    assert(chunk);
    camera_set_shader_projection_and_view(s_chunk_shader);

    if (chunk->dirty) {
        chunk_rebuild_mesh(chunk);
        chunk->dirty = false;
    }
}

void chunk_rebuild_mesh(Chunk *chunk)
{
    assert(chunk);

    for (int i = 0; i < CHUNK_SZ; ++i) {
        for (int j = 0; j < CHUNK_SZ; ++j) {
            for (int k = 0; k < CHUNK_SZ; ++k) {
                if (chunk->blocks[i][j][k].type != BLOCK_AIR) {
                    chunk_render_block(chunk, (ivec3s){ .x=i, .y=j, .z=k }, chunk->blocks[i][j][k]);
                }
            }
        }
    }
}

void chunk_render_block(Chunk *chunk, ivec3s pos, Block block)
{
    switch (block.type) {
    case BLOCK_STONE: render_stone_block(chunk, pos); break;
    case BLOCK_DIRT:  render_dirt_block(chunk, pos);  break;
    case BLOCK_GRASS: render_grass_block(chunk, pos); break;
    default:
        vwarn("invalid blocktype passed to '%s': %d'", __FUNCTION__, block.type);
        break;
    }
}

void chunk_render(Chunk *chunk)
{
    assert(chunk);
    glUseProgram(s_chunk_shader);
    glBindVertexArray(s_chunk_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_chunk_vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 chunk->mesh.vertices.count * sizeof(BlockVertex), 
                 chunk->mesh.vertices.items,
                 GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, chunk->mesh.vertices.count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void chunk_put_block(Chunk *chunk, Block block, ivec3s pos)
{
    assert(chunk);
    assert(pos.x >= 0 && pos.x < CHUNK_SZ);
    assert(pos.y >= 0 && pos.y < CHUNK_SZ);
    assert(pos.z >= 0 && pos.z < CHUNK_SZ);
    assert(chunk->blocks[pos.x][pos.y][pos.z].type == BLOCK_AIR);

    chunk->blocks[pos.x][pos.y][pos.z] = block;
    chunk->dirty = true;
}

int chunkmesh_new(ChunkMesh *mesh)
{
    assert(mesh);

    glUseProgram(s_chunk_shader);
    glBindVertexArray(s_chunk_vao);

    da_init(&mesh->vertices);

    glBindVertexArray(0);
    glUseProgram(0);
    return 0;
}

void chunkmesh_delete(ChunkMesh *mesh)
{
    assert(mesh);
    glUseProgram(s_chunk_shader);
    da_free(mesh->vertices);
    glUseProgram(0);
}

BlockVertex make_block_vertex(vec3 pos, vec3 normal, vec4 color)
{
    return (BlockVertex) {
        { pos[0], pos[1], pos[2] }, { normal[0], normal[1], normal[2] }, { color[0], color[1], color[2], color[3] }
    };
}

void render_standard_block(Chunk *chunk, ivec3s pos, vec4s color)
{
    assert(chunk);
    assert(pos.x >= 0 && pos.x < CHUNK_SZ);
    assert(pos.y >= 0 && pos.y < CHUNK_SZ);
    assert(pos.z >= 0 && pos.z < CHUNK_SZ);

    const float N = BLOCK_SIDELEN;
    vec3 v0 = { pos.x*N,     pos.y*N,     pos.z*N };
    vec3 v1 = { (pos.x+1)*N, pos.y*N,     pos.z*N };
    vec3 v2 = { (pos.x+1)*N, (pos.y+1)*N, pos.z*N };
    vec3 v3 = { pos.x*N,     (pos.y+1)*N, pos.z*N };
    vec3 v4 = { pos.x*N,     pos.y*N,     (pos.z+1)*N };
    vec3 v5 = { (pos.x+1)*N, pos.y*N,     (pos.z+1)*N };
    vec3 v6 = { (pos.x+1)*N, (pos.y+1)*N, (pos.z+1)*N };
    vec3 v7 = { pos.x*N,     (pos.y+1)*N, (pos.z+1)*N };

    // back face (-z is far plane)
    da_append(&chunk->mesh.vertices, make_block_vertex(v0, (vec3){ 0, 0, -1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v1, (vec3){ 0, 0, -1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v2, (vec3){ 0, 0, -1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v2, (vec3){ 0, 0, -1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v3, (vec3){ 0, 0, -1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v0, (vec3){ 0, 0, -1 }, &color.raw[0]));

    // front face
    da_append(&chunk->mesh.vertices, make_block_vertex(v4, (vec3){ 0, 0,  1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v5, (vec3){ 0, 0,  1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v6, (vec3){ 0, 0,  1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v6, (vec3){ 0, 0,  1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v7, (vec3){ 0, 0,  1 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v4, (vec3){ 0, 0,  1 }, &color.raw[0]));

    // left face
    da_append(&chunk->mesh.vertices, make_block_vertex(v7, (vec3){ -1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v3, (vec3){ -1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v0, (vec3){ -1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v0, (vec3){ -1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v4, (vec3){ -1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v7, (vec3){ -1, 0, 0 }, &color.raw[0]));

    // right face
    da_append(&chunk->mesh.vertices, make_block_vertex(v6, (vec3){  1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v2, (vec3){  1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v1, (vec3){  1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v1, (vec3){  1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v5, (vec3){  1, 0, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v6, (vec3){  1, 0, 0 }, &color.raw[0]));

    // top face
    da_append(&chunk->mesh.vertices, make_block_vertex(v0, (vec3){ 0, -1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v1, (vec3){ 0, -1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v5, (vec3){ 0, -1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v5, (vec3){ 0, -1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v4, (vec3){ 0, -1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v0, (vec3){ 0, -1, 0 }, &color.raw[0]));

    // bottom face
    da_append(&chunk->mesh.vertices, make_block_vertex(v3, (vec3){ 0,  1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v2, (vec3){ 0,  1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v6, (vec3){ 0,  1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v6, (vec3){ 0,  1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v7, (vec3){ 0,  1, 0 }, &color.raw[0]));
    da_append(&chunk->mesh.vertices, make_block_vertex(v3, (vec3){ 0,  1, 0 }, &color.raw[0]));
}

void render_grass_block(Chunk *chunk, ivec3s pos)
{
    assert(chunk);
    const vec4s grass_color = (vec4s){ .r=0.011, .g=0.431, .b=0.054, .a=1.0f };
    render_standard_block(chunk, pos, grass_color);
}

void render_stone_block(Chunk *chunk, ivec3s pos)
{
    assert(chunk);
    const vec4s stone_color = (vec4s){ .r=0.494, .g=0.505, .b=0.529, .a=1.0f };
    render_standard_block(chunk, pos, stone_color);
}

void render_dirt_block(Chunk *chunk, ivec3s pos)
{
    assert(chunk);
    const vec4s dirt_color = (vec4s){ .r=0.431, .g=0.274, .b=0.145, .a=1.0f };
    render_standard_block(chunk, pos, dirt_color);
}
