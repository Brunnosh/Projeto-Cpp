#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <chunk.h>
#include <future>



class Camera;
struct RaycastHit;





class World {
private:
    std::unordered_map<glm::ivec3, Chunk, Vec3Hash> WorldData;

    //std::unordered_map<glm::ivec3, Chunk*, Vec3Hash> activeChunks;

    std::unordered_map<std::pair<int,int>, int, PairHash> highestChunkY;

public:
    std::vector<std::future<std::pair<glm::ivec3, Chunk>>> chunkFutures;
    std::vector<glm::ivec3> chunkQueue;
    std::unordered_set<glm::ivec3, Vec3Hash> chunkRequested;
    float sunAngle = 0.0f; // Começa no leste
    float sunSpeed = 5.0f;




public:
    World();

    void World::update(Camera& camera, float deltaTime, unsigned int modelLoc, int& drawCallCount);

    void World::genWorld(Camera& camera, unsigned int modelLoc);

    void World::renderWorld(unsigned int modelLoc, int &drawCallCount);

    void World::calculateLight();

    void World::tick();

    std::optional<RaycastHit> World::isBlockAir(glm::ivec3 blockPos);

    void World::removeBlock(RaycastHit& hit);

    void World::placeBlock(Camera& camera, RaycastHit& hit, Block blockToPlace);



    int getMaxChunkY(int x, int z) {
        std::pair<int, int> xzKey = { x, z };
        auto it = highestChunkY.find(xzKey);
        if (it != highestChunkY.end() ) {
            return highestChunkY[xzKey];
        }
        
    }

   
    int World::getNumberChunks() {
        return WorldData.size();
    }
};

#endif
