#include "block.h"
#include <vector>

std::unordered_map<BlockType, Block> Blocks = {
    { BlockType::AIR,   Block(BlockType::AIR) },
    { BlockType::GRASS, Block(BlockType::GRASS) },
    { BlockType::DIRT,  Block(BlockType::DIRT) }
};

std::unordered_map<BlockType, std::vector<uint8_t>> blockTextures = {
    //                    N  S  E  W  T  B
    { BlockType::GRASS, { 2, 2, 2, 2, 3, 1 } },
    { BlockType::DIRT,  { 1, 1, 1, 1, 1, 1 } },
    { BlockType::AIR,   { 0, 0, 0, 0, 0, 0 } } // opcional pra "sem textura"
};

//declarar blocos
