#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <unordered_map>

enum class BlockFace {
    SOUTH, //frente
    NORTH, //trás
    EAST, // direita
    WEST, //esq
    TOP, 
    BOTTOM
};

struct UV {
    float uMin, vMin;
    float uMax, vMax;
};

enum class BlockType {
    AIR,
    GRASS,
    DIRT,
    
    AMOUNT
};


class Block {
private:
    std::string name;
    BlockType type;


    //Atlas texture coords on each face, set at constructor.
    short topX, topY;

    short northX, northY;

    short southX, southY;

    short eastX, eastY;

    short westX, westY;

    short botX, botY;
    //---------------------------------------------------

    std::unordered_map<BlockFace, UV> faceUVs;

public:


    //one texture block (DIRT, STONE)
    Block(std::string name, BlockType type, short topX, short topY):
        name(name), type(type), topX(topX), topY(topY) {
        northX = southX = eastX = westX = botX = topX;
        northY = southY = eastY = westY = botY = topY;
        calculateUVs();
    }

    //3 texture block (GRASS)
    Block(std::string name, BlockType type, short topX, short topY, short sideX, short sideY, short botX, short botY):
        name(name), type(type), topX(topX), topY(topY), botX(botX), botY(botY){
        northX = southX = eastX = westX = sideX;
        northY = southY = eastY = westY = sideY;
        calculateUVs();

    }

    //create more constructors depending on how many faces the block has

    UV computeUV(short x, short y) {
        float step = 16.0f / 256.0f;

        float uMin = (x - 1) * step;
        float uMax = x * step;

        float vMax = 1.0f - ( (y - 1) * step);
        float vMin = vMax - step;

        return { uMin, vMin, uMax, vMax };
    }

    void calculateUVs() {
        faceUVs[BlockFace::TOP] = computeUV(topX, topY);
        faceUVs[BlockFace::BOTTOM] = computeUV(botX, botY);
        faceUVs[BlockFace::NORTH] = computeUV(northX, northY);
        faceUVs[BlockFace::SOUTH] = computeUV(southX, southY);
        faceUVs[BlockFace::EAST] = computeUV(eastX, eastY);
        faceUVs[BlockFace::WEST] = computeUV(westX, westY);
    }



    std::string getName() const { return name; }
    BlockType getType() const { return type; }

    UV getUV(BlockFace face) const {
        return faceUVs.at(face);
    }



};

extern std::vector<Block> blockList;

inline const Block& getBlock(BlockType type) {
    return blockList[static_cast<int>(type)];
}

#endif // BLOCK_H