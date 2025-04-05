#include "block.h"
#include <vector>

std::vector<Block> blockList = {
                      //UV COORDS   XMIN XMAX  YMIN  YMAX
    Block("Grass", BlockType::AIR, 0.0f, 0.0f, 0.0f, 0.0f),
    Block("Dirt", BlockType::GRASS, 0.25f, 0.5f, 0.0f, 1.0f),
    Block("Stone", BlockType::DIRT, 0.0f, 0.0625f, 0.9375f, 1.0f)
    // Adicione mais blocos aqui conforme o enum crescer
};