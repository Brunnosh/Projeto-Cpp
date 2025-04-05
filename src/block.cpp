#include "block.h"
#include <vector>

//ATLAS TEXTURE ORDER ->
    //DIRT BOTTOM = X( 0-> 1) Y(1 -> 0.9375)
    //GRASS BLOCK SIDE = 2
    //GRASS BLOCK TOP = 3


std::vector<Block> blockList = {
 
    Block("AIR", BlockType::AIR, 0,0),
    Block("Grass", BlockType::GRASS, 1,1),
    Block("Dirt Block", BlockType::DIRT, 1,1)
    
};

