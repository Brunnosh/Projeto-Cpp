#include <chunk.h>
#include <shader.h>
#include <world_gen.h>

#include <chrono>
#include <iostream>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <queue>
#include <array>

Chunk::Chunk(glm::ivec3 pos) {
    worldPos = pos;
}

Chunk::~Chunk() {}

Block &Chunk::getBlock(int x, int y, int z) {
    return chunkData[x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y];
}

bool Chunk::isAirAt(int x, int y, int z) {

  


    int index = x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y;

   

    return chunkData[index].getType() == BlockType::AIR;

}

bool Chunk::isChunkEmpty() {


    for (const Block& block : chunkData) {
        if (block.getType() != BlockType::AIR) {
            isEmpty = false;
            return false;
        }
    }
    isEmpty = true;
    return true;

}



