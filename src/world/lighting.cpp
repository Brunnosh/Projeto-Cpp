#include <lighting.h>


namespace Lighting {
    std::stack<std::pair<int, int>> pendingColumns;
    std::set<std::pair<int, int>> columnsPendingControl;

    void queueColumnForLightingUpdate(int x, int z) {
        std::pair xzCoord = std::pair(x, z);    

        if (columnsPendingControl.find(xzCoord) == columnsPendingControl.end()) {
            pendingColumns.push(xzCoord);
            Lighting::columnsPendingControl.insert(xzCoord);
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

           

            if (!it->second.chunk || it->second.state != chunkState::READY) {
                // Chunk ainda não pronto
                return false;
            }

            chunkY--;
        }
    }

}