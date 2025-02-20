#include "cube_vertices.h"
BlockVertex g_cube_vertices[36] = {
    // back face
    { { -0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },

    // front face
    { { -0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },

    // left face
    { { -0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },

    // right face
    { {  0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },

    // bottom face
    { { -0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },

    // top face
    { { -0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.88f, 0.04f, 0.06f, 1.0f } },
};
