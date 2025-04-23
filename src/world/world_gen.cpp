#include <world_gen.h>
#include <block.h>
#include <chunk.h>


void World_Gen::generateChunkData( Chunk & chunk) {
    //Futuramente usar WorldPos junto com a seed/Noise para gerar os blocos do chunk
    std::vector<Block> tempVec;
    //teste chunk 15x15x15 solido
    //std::cout << "GERANDO CHUNK X: " << chunkCoords.x<< ", Y:  " << chunkCoords.y << ", Z: "<< chunkCoords.z << "\n";
    if (chunk.worldPos == glm::ivec3(0, 3, 0)) {
        for (int i = 0; i < CHUNKSIZE * CHUNKSIZE * CHUNKSIZE; i++) {
            tempVec.push_back(Blocks[BlockType::AIR]);
        }
        chunk.chunkData = tempVec;
        
        return;
    }


    for (char x = 0; x < CHUNKSIZE; x++) {
        for (char z = 0; z < CHUNKSIZE; z++) {
            for (char y = 0; y < CHUNKSIZE; y++) {
                if ((chunk.worldPos.y * CHUNKSIZE) < CHUNKSIZE * 4 && (chunk.worldPos.y * CHUNKSIZE) > 0) {
                    tempVec.push_back(Blocks[BlockType::GRASS]);
                }
                else
                {
                    tempVec.push_back(Blocks[BlockType::AIR]);
                }
            }
        }
    }
    chunk.chunkData = tempVec;
    return;
}