#include "block.h"

const Block BLOCKS[static_cast<int>(BlockType::COUNT)] = {
    { BlockType::AIR, "AIR", 0.0f, 0.0f, 0.0f, 0.0f },
    { BlockType::DIRT, "Dirt Block",   0.0f,     0.9375f, 0.0625f, 1.0f },
    { BlockType::GRASS, "Grass Block", 0.125f,   0.9375f, 0.1875f, 1.0f }
};

const Block& getBlock(BlockType type) {
    return BLOCKS[static_cast<int>(type)];
}

