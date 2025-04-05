#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>

enum class Face { FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM };

enum class BlockType {
    AIR,
    GRASS,
    DIRT,
    
    AMOUNT
};


class Block {
private:
    float uvxmin;
    float uvxmax;
    float uvymin;
    float uvymax;

    std::string name;
    BlockType type;


public:


    Block(std::string name, BlockType type, float uvxmin, float uvxmax, float uvymin, float uvymax)
        : name(name), type(type), uvxmin(uvxmin), uvxmax(uvxmax), uvymin(uvymin), uvymax(uvymax) {
    }
    float getUVXMin() const { return uvxmin; }
    float getUVXMax() const { return uvxmax; }
    float getUVYMin() const { return uvymin; }
    float getUVYMax() const { return uvymax; }

    std::string getName() const { return name; }
    BlockType getType() const { return type; }


};

extern std::vector<Block> blockList;

#endif // BLOCK_H