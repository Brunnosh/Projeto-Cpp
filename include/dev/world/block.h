#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>


enum class BlockType : uint8_t  {
    AIR = 0, 
    GRASS,
    DIRT,
    
    AMOUNT
};

static const char* BlockTypeNames[] = {
    "Air",
    "Grass",
    "Dirt"

};

enum FACE { //order used for texture index in chunk.cpp/block.cpp
    NORTH,
    SOUTH,
    EAST,
    WEST,
    TOP,
    BOTTOM
};

struct UV {
    float uMin, vMin;
    float uMax, vMax;
};



class Block {
private:
    BlockType type;
    uint8_t skyLight:4;
    uint8_t blockLight:4;

public:

    Block() {
        std::cerr << "[ERRO] Construtor default de Block foi chamado!" << std::endl;
        throw std::runtime_error("Uso indevido do construtor default de Block.");
    }

    Block(BlockType type) :type(type) { 
        skyLight = 0;
        blockLight = 0;
    }

    
    BlockType getType() const {
        return type;
    }

    inline const char* getTypeToString() {
        return BlockTypeNames[static_cast<int>(this->type)];
    }


    UV computeUV(short position) {
        float step = 16.0f / 256.0f;

        short index = position - 1;
        short x = index % 16; // coluna (0 a 15)
        short y = index / 16; // linha (0 a 15)

        float uMin = x * step;
        float uMax = (x + 1) * step;

        float vMax = 1.0f - y * step;
        float vMin = vMax - step;

        return { uMin, vMin, uMax, vMax };
    }

    uint8_t getSkyLight() {
        return skyLight;
    }

    uint8_t getBlockLight() {
        return blockLight;
    }

    void setSkyLight(uint8_t level) {
        skyLight = level;
    }
    void setBlockLight(uint8_t level) {
        blockLight = level;
    }

};

extern std::unordered_map<BlockType, Block> Blocks;

extern std::unordered_map<BlockType, std::vector<uint8_t>> blockTextures;

extern std::unordered_map<BlockType, std::vector<UV>> blockUVs;

void initBlockUVs();
//ATLAS TEXTURE ORDER ->
    //DIRT BOTTOM = X( 0-> 1) Y(1 -> 0.9375)
    //GRASS BLOCK SIDE = 2
    //GRASS BLOCK TOP = 3


    //                              topx, topy  sidex  sidey   botx   boty
    //Block("Grass", BlockType::GRASS,  3    ,3 ,  2    , 2     , 1    , 1),


//Funcoes para depois
/*


void calculateUVs() {
    faceUVs[BlockFace::TOP] = computeUV(topX, topY);
    faceUVs[BlockFace::BOTTOM] = computeUV(botX, botY);
    faceUVs[BlockFace::NORTH] = computeUV(northX, northY);
    faceUVs[BlockFace::SOUTH] = computeUV(southX, southY);
    faceUVs[BlockFace::EAST] = computeUV(eastX, eastY);
    faceUVs[BlockFace::WEST] = computeUV(westX, westY);
}









*/
#endif 