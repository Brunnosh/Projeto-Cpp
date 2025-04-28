#include <lighting.h>
#include <renderer.h>

namespace Lighting {
    std::queue<std::pair<int, int>> pendingColumns;
    std::set<std::pair<int, int>> columnsPendingControl;


    void processPendingColumns(World& world, Renderer& worldRenderer) {
        int pendingCount = pendingColumns.size();

        // Opcional: limitar quantas processa por frame
        int processLimit = 5;
        int processed = 0;

        for (int i = 0; i < pendingCount && processed < processLimit; ++i) {
            std::pair xz = pendingColumns.front();
            pendingColumns.pop();

            if (!checkChunkColumn(world, xz)) {
                // Coluna ainda não pronta, volta para pending
                columnsPendingControl.erase(xz);
                pendingColumns.push(xz);
                columnsPendingControl.insert(xz);
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
            if (it == worldData.end()) {
                // Se não existe chunk, está tudo bem, terminamos aqui
                return true;
            }


            //MUDAR PRA CHUNKSTATE::GENERATED
            if (!it->second.chunk || it->second.state != chunkState::QUEUED_LIGHT_UPDATE) {
                // Chunk ainda não pronto
                return false;
            }

            chunkY--;
        }
    }




    void Lighting::initializeLightColumn(World& world, const std::pair<int, int>& xz, Renderer& worldRenderer) {
        auto& worldData = world.getWorldDataRef();
        int highestY = world.getMaxChunkY(xz.first, xz.second);
        std::queue<glm::ivec3> tempQueue;

        bool blocked[CHUNKSIZE][CHUNKSIZE] = {}; // Inicializa todas as colunas como não bloqueadas

        int chunkY = highestY;
        while (true) {
            glm::ivec3 chunkPos = { xz.first, chunkY, xz.second };
            tempQueue.push(chunkPos);

            auto it = worldData.find(chunkPos);
            if (it == worldData.end() || !it->second.chunk) {
                break;
            }

            it->second.state = chunkState::DIRTY;
            Chunk& chunk = *it->second.chunk;

            // Para cada posição no chunk
            for (int y = CHUNKSIZE - 1; y >= 0; --y) {  // De cima para baixo dentro do chunk
                for (int lx = 0; lx < CHUNKSIZE; ++lx) {
                    for (int lz = 0; lz < CHUNKSIZE; ++lz) {
                        Block& block = chunk.getBlock(lx, y, lz);

                        if (!blocked[lx][lz]) {
                            block.setSkyLight(15); // Ainda recebendo luz

                            if (block.getType() != BlockType::AIR) {
                                blocked[lx][lz] = true; // A partir daqui, a coluna está bloqueada
                            }
                        }
                        else {
                            block.setSkyLight(0); // Coluna já bloqueada, sombra
                        }
                    }
                }
            }

            chunkY--; // Passa para o próximo chunk abaixo
        }

        worldRenderer.markChunkRemesh(tempQueue);
    }
}