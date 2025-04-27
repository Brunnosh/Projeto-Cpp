#include <lighting.h>
#include <iostream>

void Lighting::initializeChunkLight(const std::unordered_map<std::pair<int, int>, int, PairHash>& highestChunkY, const std::unordered_map<glm::ivec3, chunkObject, Vec3Hash>& worldData) {


    for (auto& [xzKey, highestY] : highestChunkY) {
        int x = xzKey.first;
        int z = xzKey.second;

        bool blocked = false; // Se encontramos algum bloco sólido

        int chunkY = highestY;
        while (true) {
            glm::ivec3 chunkPos = { x, chunkY, z };

            auto it = worldData.find(chunkPos);
            if (it == worldData.end() || !it->second.chunk) {
                // Se não existe chunk aqui, paramos de descer
                break;
            }

            Chunk& chunk = *it->second.chunk;

            for (int y = CHUNKSIZE - 1; y >= 0; --y) { // De cima para baixo
                for (int lx = 0; lx < CHUNKSIZE; ++lx) {
                    for (int lz = 0; lz < CHUNKSIZE; ++lz) {
                        Block& block = chunk.getBlock(lx, y, lz);

                        if (!blocked) {
                            if (block.getType() == BlockType::AIR) {
                                block.setSkyLight(15); // Recebe luz total do céu
                            }
                            else {
                                blocked = true; // Encontramos um bloco sólido
                                block.setSkyLight(0);
                            }
                        }
                        else {
                            block.setSkyLight(0); // Tudo abaixo está na sombra
                        }
                    }
                }
            }

            --chunkY; // Descer para o próximo chunk na coluna
        }
    }
}
