#include <lighting.h>
#include <renderer.h>

namespace Lighting {
    std::stack<std::pair<int, int>> pendingColumns;
    std::set<std::pair<int, int>> columnsPendingControl;


    void processPendingColumns(World& world, Renderer& worldRenderer) {
        int pendingCount = pendingColumns.size();

        // Opcional: limitar quantas processa por frame
        int processLimit = 5;
        int processed = 0;

        for (int i = 0; i < pendingCount && processed < processLimit; ++i) {
            std::pair xz = pendingColumns.top();
            pendingColumns.pop();

            if (!checkChunkColumn(world, xz)) {
                // Coluna ainda não pronta, volta para pending
                pendingColumns.push(xz);
                continue;
            }

            // Coluna está pronta, inicializa luz
            columnsPendingControl.erase(xz);
            initializeLightColumn(world, xz, worldRenderer);
            processed++;
        }
    }


    void queueColumnForLightingUpdate(int x, int z) {
        std::pair xzCoord = std::pair(x, z);    

        if (columnsPendingControl.find(xzCoord) == columnsPendingControl.end()) {
            pendingColumns.push(xzCoord);
            columnsPendingControl.insert(xzCoord);
        }
        
    }

    bool checkChunkColumn(World& world, const std::pair<int, int>& xz) {
        auto& worldData = world.getWorldDataRef();
        int highestY = world.getMaxChunkY(xz.first, xz.second);

        int chunkY = highestY;
        while (true) {
            glm::ivec3 chunkPos = { xz.first, chunkY, xz.second };

            

            auto it = worldData.find(chunkPos);
            if (it == worldData.end() ) {
                // Se não existe chunk, está tudo bem, terminamos aqui
                return true;
            }

           
            //MUDAR PRA CHUNKSTATE::GENERATED
            if (!it->second.chunk || it->second.state != chunkState::READY) {
                // Chunk ainda não pronto
                return false;
            }

            chunkY--;
        }
    }




    void initializeLightColumn(World& world, const std::pair<int, int>& xz, Renderer& worldRenderer) {
        auto& worldData = world.getWorldDataRef();
        int highestY = world.getMaxChunkY(xz.first, xz.second);
        std::queue<glm::ivec3> tempQueue;

        int chunkY = highestY;
        Chunk* aboveChunk = nullptr; // Ponteiro para o chunk de cima

        while (true) {
            glm::ivec3 chunkPos = { xz.first, chunkY, xz.second };
            tempQueue.push(chunkPos);

            auto it = worldData.find(chunkPos);
            if (it == worldData.end() || !it->second.chunk) {
                // Se não existe chunk aqui, para o loop
                break;
            }

            Chunk& chunk = *it->second.chunk;

            // Um bloqueio separado para cada (lx, lz)
            bool blocked[CHUNKSIZE][CHUNKSIZE] = {};

            if (chunkY == highestY) {
                // Primeiro chunk (mais alto): sol direto
                for (int y = CHUNKSIZE - 1; y >= 0; --y) {
                    for (int lx = 0; lx < CHUNKSIZE; ++lx) {
                        for (int lz = 0; lz < CHUNKSIZE; ++lz) {
                            Block& block = chunk.getBlock(lx, y, lz);

                            if (!blocked[lx][lz]) {
                                if (block.getType() == BlockType::AIR) {
                                    block.setSkyLight(15);
                                }
                                else {
                                    blocked[lx][lz] = true;
                                    block.setSkyLight(15);
                                }
                            }
                            else {
                                block.setSkyLight(0);
                            }
                        }
                    }
                }
            }
            else {
                // Chunks abaixo do mais alto
                if (chunk.isEmpty && aboveChunk) {
                    // Verifica se toda a face inferior do chunk de cima tem luz 15
                    bool allAboveFaceFullLight = true;

                    for (int lx = 0; lx < CHUNKSIZE && allAboveFaceFullLight; ++lx) {
                        for (int lz = 0; lz < CHUNKSIZE && allAboveFaceFullLight; ++lz) {
                            Block& aboveBlock = aboveChunk->getBlock(lx, 0, lz);
                            if (aboveBlock.getSkyLight() < 15) {
                                allAboveFaceFullLight = false;
                            }
                        }
                    }

                    if (allAboveFaceFullLight) {
                        // Preenche o chunk inteiro com luz 15
                        for (int y = CHUNKSIZE - 1; y >= 0; --y) {
                            for (int lx = 0; lx < CHUNKSIZE; ++lx) {
                                for (int lz = 0; lz < CHUNKSIZE; ++lz) {
                                    chunk.getBlock(lx, y, lz).setSkyLight(15);
                                }
                            }
                        }
                        // Como tudo recebeu luz 15, não precisamos alterar 'blocked'
                    }
                    else {
                        // Caso a face superior não esteja toda iluminada, checa por coluna
                        for (int y = CHUNKSIZE - 1; y >= 0; --y) {
                            for (int lx = 0; lx < CHUNKSIZE; ++lx) {
                                for (int lz = 0; lz < CHUNKSIZE; ++lz) {
                                    Block& block = chunk.getBlock(lx, y, lz);
                                    Block& aboveBlock = aboveChunk->getBlock(lx, (y == CHUNKSIZE - 1) ? 0 : (y + 1), lz);

                                    if (!blocked[lx][lz]) {
                                        if (block.getType() == BlockType::AIR && aboveBlock.getSkyLight() == 15) {
                                            block.setSkyLight(15);
                                        }
                                        else {
                                            blocked[lx][lz] = true;
                                            block.setSkyLight(15);
                                        }
                                    }
                                    else {
                                        block.setSkyLight(0);
                                    }
                                }
                            }
                        }
                    }
                }
                else if (aboveChunk) {
                    // Chunk não é vazio: checa normalmente por coluna
                    for (int y = CHUNKSIZE - 1; y >= 0; --y) {
                        for (int lx = 0; lx < CHUNKSIZE; ++lx) {
                            for (int lz = 0; lz < CHUNKSIZE; ++lz) {
                                Block& block = chunk.getBlock(lx, y, lz);
                                Block& aboveBlock = aboveChunk->getBlock(lx, (y == CHUNKSIZE - 1) ? 0 : (y + 1), lz);

                                if (!blocked[lx][lz]) {
                                    if (block.getType() == BlockType::AIR && aboveBlock.getSkyLight() == 15) {
                                        block.setSkyLight(15);
                                    }
                                    else {
                                        blocked[lx][lz] = true;
                                        block.setSkyLight(15);
                                    }
                                }
                                else {
                                    block.setSkyLight(0);
                                }
                            }
                        }
                    }
                }
            }

            // Atualiza o chunk acima para o atual
            aboveChunk = &chunk;
            chunkY--; // Anda para o chunk debaixo (pode ser negativo)
        }

        // Marca todos os chunks que atualizamos para redesenhar
        worldRenderer.markChunkDirty(tempQueue);
    }











}