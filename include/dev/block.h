#ifndef BLOCK_H
#define BLOCK_H


enum class BlockType {
    AIR, // = 0
    GRASS,
    DIRT,
    
    AMOUNT
};







class Block {
private:
    BlockType type;
    bool active;

public:
    Block(BlockType type, bool active ):type(type), active(active){}

    

    bool isActive() {
        return active;
    }
    void setActive(bool state) {
        active = state;
    }
        

};


extern std::vector<Block> Blocks;



//ATLAS TEXTURE ORDER ->
    //DIRT BOTTOM = X( 0-> 1) Y(1 -> 0.9375)
    //GRASS BLOCK SIDE = 2
    //GRASS BLOCK TOP = 3


    //                              topx, topy  sidex  sidey   botx   boty
    //Block("Grass", BlockType::GRASS,  3    ,3 ,  2    , 2     , 1    , 1),


//Funcoes para depois
/*
UV computeUV(short x, short y) {
    float step = 16.0f / 256.0f;

    float uMin = (x - 1) * step;
    float uMax = x * step;

    float vMax = 1.0f - ((y - 1) * step);
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


struct UV {
    float uMin, vMin;
    float uMax, vMax;
};






*/
#endif 