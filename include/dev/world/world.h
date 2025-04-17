#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chunk.h>
#include <future>
#include <camera.h>


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

    std::optional<RaycastHit> isBlockAir(glm::ivec3 blockPos);

    void removeBlock(RaycastHit& hit);

    void placeBlock(Camera& camera, RaycastHit& hit, Block blockToPlace);
   
    int getNumberChunks() {
        return WorldData.size();
    }

};

#endif
