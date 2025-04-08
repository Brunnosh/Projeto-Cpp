#include "block.h"
#include <vector>



std::unordered_map<BlockType, Block> Blocks = {
    { BlockType::AIR,   Block(BlockType::AIR) },
    { BlockType::GRASS, Block(BlockType::GRASS) },
    { BlockType::DIRT,  Block(BlockType::DIRT) }
};

std::unordered_map<BlockType, std::vector<uint8_t>> blockTextures = {
    //                    N  S  E  W  T  B  (ENUM FACE)
    { BlockType::GRASS, { 2, 2, 2, 2, 3, 1 } },
    { BlockType::DIRT,  { 1, 1, 1, 1, 1, 1 } },
    { BlockType::AIR,   { 0, 0, 0, 0, 0, 0 } } // opcional pra "sem textura"
};


std::unordered_map<BlockType, std::vector<UV>> blockUVs = {};
//                    N  S  E  W  T  B  (ENUM FACE)


void initBlockUVs() {
    for (const auto& [type, textures] : blockTextures) {
        std::vector<UV> uvs;
        for (uint8_t texIndex : textures) {
            uvs.push_back(Blocks[type].computeUV(texIndex));
        }
        blockUVs[type] = uvs;
    }

}