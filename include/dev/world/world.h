#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chunk.h>
#include <camera.h>
#include <future>


class World {
private:
    std::unordered_map<glm::ivec3, Chunk, Vec3Hash> WorldData;

public:
    std::vector<std::future<std::pair<glm::ivec3, Chunk>>> chunkFutures;
    std::vector<glm::ivec3> chunkQueue;
    std::unordered_set<glm::ivec3, Vec3Hash> chunkRequested;




public:
    World();

    void update(Camera& camera, float deltaTime, unsigned int modelLoc);

    void genWorld(Camera& camera, unsigned int modelLoc);
    void tick();
    int getNumberChunks() {
        return WorldData.size();
    }

    std::optional<RaycastHit> isBlockAir(glm::ivec3 blockPos);

    void removeBlock(RaycastHit& hit) {
        int max = CHUNKSIZE - 1;

        int index = hit.blockRelativePos.x * CHUNKSIZE * CHUNKSIZE + hit.blockRelativePos.z * CHUNKSIZE + hit.blockRelativePos.y;
        hit.chunk->chunkData[index] = Blocks[BlockType::AIR];
        hit.chunk->dirty = true;

        auto tryMark = [&](glm::ivec3 offset) {
            auto neighborPos = hit.chunk->worldPos + offset;
            auto it = WorldData.find(neighborPos);
            if (it != WorldData.end()) {
                it->second.dirty = true;
            }
            };

        if (hit.blockRelativePos.x == 0)         tryMark(glm::ivec3(-1, 0, 0));
        else if (hit.blockRelativePos.x == max)  tryMark(glm::ivec3(1, 0, 0));

        if (hit.blockRelativePos.y == 0)         tryMark(glm::ivec3(0, -1, 0));
        else if (hit.blockRelativePos.y == max)  tryMark(glm::ivec3(0, 1, 0));

        if (hit.blockRelativePos.z == 0)         tryMark(glm::ivec3(0, 0, -1));
        else if (hit.blockRelativePos.z == max)  tryMark(glm::ivec3(0, 0, 1));

    }
   
};

#endif
