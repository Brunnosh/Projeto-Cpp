#pragma once
#include <string>

enum class BlockType {
    AIR,
    DIRT,
    GRASS,

    COUNT // usado pra saber quantos blocos existem
};


struct Block {
    BlockType type;
    std::string name;
    //coordenadas UV no atlas
    float uvxMin, uvyMin;
    float uvxMax, uvyMax;
};

//Falando pro compilador que esse array existe, nao precisa criar outro
extern const Block BLOCKS[static_cast<int>(BlockType::COUNT)];

const Block& getBlock(BlockType type);