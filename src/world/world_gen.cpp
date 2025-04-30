#include <world_gen.h>
#include <block.h>
#include <chunk.h>


void World_Gen::generateChunkData( Chunk & chunk) {
    chunk.chunkData.reserve(CHUNKSIZE* CHUNKSIZE* CHUNKSIZE);
    //Futuramente usar WorldPos junto com a seed/Noise para gerar os blocos do chunk
   
    //teste chunk 15x15x15 solido
    //std::cout << "GERANDO CHUNK X: " << chunkCoords.x<< ", Y:  " << chunkCoords.y << ", Z: "<< chunkCoords.z << "\n";
    if (chunk.worldPos == glm::ivec3(0, 3, 0)) {
        for (int i = 0; i < CHUNKSIZE * CHUNKSIZE * CHUNKSIZE; i++) {
            chunk.chunkData.push_back(Blocks[BlockType::AIR]);
        }
        
        
        return;
    }


    for (char x = 0; x < CHUNKSIZE; x++) {
        for (char z = 0; z < CHUNKSIZE; z++) {
            for (char y = 0; y < CHUNKSIZE; y++) {
                if ((chunk.worldPos.y * CHUNKSIZE) < CHUNKSIZE * 4 && (chunk.worldPos.y * CHUNKSIZE) > 0) {
                    chunk.chunkData.push_back(Blocks[BlockType::GRASS]);
                }
                else
                {
                    chunk.chunkData.push_back(Blocks[BlockType::AIR]);
                }
            }
        }
    }
    
    return;
}