#ifndef BLOCK_H
#define BLOCK_H

#include "common.h"

typedef enum BlockType {
    BLOCK_AIR = 0,
    BLOCK_DIRT,
    BLOCK_STONE,
    BLOCK_GRASS,
    BLOCK_SAND,
    BLOCK_WOOL,
    BLOCK_TYPE_COUNT
} BlockType;

typedef struct Block {
    BlockType type;
} Block;

#endif
