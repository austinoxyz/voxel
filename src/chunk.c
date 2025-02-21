#include "chunk.h"
#include "shader.h"
#include "camera.h"
#include "lightsource.h"

void chunk_render_block(Chunk *chunk, ivec3s pos, Block block);

static const vec4s s_block_color[BLOCK_TYPE_COUNT] = {
    (vec4s){ .r=0.000, .g=0.000, .b=0.000, .a=0.0f },
    (vec4s){ .r=0.011, .g=0.431, .b=0.054, .a=1.0f },
    (vec4s){ .r=0.494, .g=0.505, .b=0.529, .a=1.0f },
    (vec4s){ .r=0.431, .g=0.274, .b=0.145, .a=1.0f },
    (vec4s){ .r=1.000, .g=0.937, .b=0.769, .a=1.0f },
    (vec4s){ .r=0.929, .g=0.917, .b=0.882, .a=1.0f }
};

int chunk_new(Chunk *chunk, ivec2s pos, World *world, Shader shader, GLuint vao, GLuint vbo)
{
    assert(chunk);

    da_init(&chunk->vertices);

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
    chunk->world = world;
    chunk->shader = shader;
    chunk->vao = vao;
    chunk->vbo = vbo;
    return 0;
}

void chunk_delete(Chunk *chunk)
{
    assert(chunk);

    da_free(chunk->vertices);

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
    camera_set_shader_projection_and_view(chunk->shader);
    shader_set_uniform_vec3(chunk->shader, "viewPos", camera_get()->pos);

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

BlockVertex make_block_vertex(vec3 pos, vec3 normal, vec4 color, BlockType type)
{
    return (BlockVertex) {
        { pos[0], pos[1], pos[2] }, 
        { normal[0], normal[1], normal[2] }, 
        { color[0], color[1], color[2], color[3] },
        type
    };
}

void chunk_render_block(Chunk *chunk, ivec3s pos, Block block)
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

    vec4s color = s_block_color[block.type];


    // back face (-z is far plane)
    if (pos.z == 0 || chunk->blocks[pos.x][pos.y][pos.z-1].type == BLOCK_AIR) {
        da_append(&chunk->vertices, make_block_vertex(v0, (vec3){ 0, 0, -1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v2, (vec3){ 0, 0, -1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v1, (vec3){ 0, 0, -1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v2, (vec3){ 0, 0, -1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v0, (vec3){ 0, 0, -1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v3, (vec3){ 0, 0, -1 }, &color.raw[0], block.type));
    }

    // front face
    if (pos.z == CHUNK_SZ-1 || chunk->blocks[pos.x][pos.y][pos.z+1].type == BLOCK_AIR) {
        da_append(&chunk->vertices, make_block_vertex(v4, (vec3){ 0, 0,  1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v5, (vec3){ 0, 0,  1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v6, (vec3){ 0, 0,  1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v6, (vec3){ 0, 0,  1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v7, (vec3){ 0, 0,  1 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v4, (vec3){ 0, 0,  1 }, &color.raw[0], block.type));
    }

    // left face
    if (pos.x == 0 || chunk->blocks[pos.x-1][pos.y][pos.z].type == BLOCK_AIR) {
        da_append(&chunk->vertices, make_block_vertex(v7, (vec3){ -1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v3, (vec3){ -1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v0, (vec3){ -1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v0, (vec3){ -1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v4, (vec3){ -1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v7, (vec3){ -1, 0, 0 }, &color.raw[0], block.type));
    }

    // right face
    if (pos.x == CHUNK_SZ-1 || chunk->blocks[pos.x+1][pos.y][pos.z].type == BLOCK_AIR) {
        da_append(&chunk->vertices, make_block_vertex(v6, (vec3){  1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v1, (vec3){  1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v2, (vec3){  1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v1, (vec3){  1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v6, (vec3){  1, 0, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v5, (vec3){  1, 0, 0 }, &color.raw[0], block.type));
    }

    // bottom face
    if (pos.y == 0 || chunk->blocks[pos.x][pos.y-1][pos.z].type == BLOCK_AIR) {
        da_append(&chunk->vertices, make_block_vertex(v0, (vec3){ 0, -1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v1, (vec3){ 0, -1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v5, (vec3){ 0, -1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v5, (vec3){ 0, -1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v4, (vec3){ 0, -1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v0, (vec3){ 0, -1, 0 }, &color.raw[0], block.type));
    }

    // top face
    if (pos.y == CHUNK_SZ-1 || chunk->blocks[pos.x][pos.y+1][pos.z].type == BLOCK_AIR) {
        da_append(&chunk->vertices, make_block_vertex(v3, (vec3){ 0,  1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v6, (vec3){ 0,  1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v2, (vec3){ 0,  1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v6, (vec3){ 0,  1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v3, (vec3){ 0,  1, 0 }, &color.raw[0], block.type));
        da_append(&chunk->vertices, make_block_vertex(v7, (vec3){ 0,  1, 0 }, &color.raw[0], block.type));
    }
}

void chunk_render(Chunk *chunk, float dt)
{
    assert(chunk);
    UNUSED(dt);
    glUseProgram(chunk->shader);
    glBindVertexArray(chunk->vao);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                 chunk->vertices.count * sizeof(BlockVertex), 
                 chunk->vertices.items,
                 GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, chunk->vertices.count);
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
