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

Block Chunk::getBlock(int x, int y, int z) {
    return chunkData[x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y];
}

bool Chunk::isAirAt(int x, int y, int z, std::vector<Block>* chunkData, std::vector<Block>* nextChunkData) {
    if (x >= 0 && x < CHUNKSIZE &&
        y >= 0 && y < CHUNKSIZE &&
        z >= 0 && z < CHUNKSIZE) {

        // Dentro do chunk atual
        int index = x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y;
        return (*chunkData)[index].getType() == BlockType::AIR;
    }
    else {
        
        // Acessar chunk vizinho — ajustar a coordenada
        int nx = (x + CHUNKSIZE) % CHUNKSIZE;
        int ny = (y + CHUNKSIZE) % CHUNKSIZE;
        int nz = (z + CHUNKSIZE) % CHUNKSIZE;

        int index = nx * CHUNKSIZE * CHUNKSIZE + nz * CHUNKSIZE + ny;
        return (*nextChunkData)[index].getType() == BlockType::AIR;
    }

    // Não tem chunk vizinho  considere ar (para forçar renderização da face)
    return true;
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



