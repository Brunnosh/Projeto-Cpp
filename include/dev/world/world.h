#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <chunk.h>
#include <future>
#include <queue>
#include <set>



class Camera;
struct RaycastHit;

enum class chunkState {
    UNLOADED, //Chunk does not exist (chunk = nullptr)
    LOADING, // in process of loading/generating chunkData.
    GENERATED, //chunkData is loaded, chunk is not meshed and not prepared.
    
    READY // Chunk is meshed and light is calculated -> ready to render.

};

struct chunkObject {
    std::shared_ptr<Chunk> chunk = nullptr;
    chunkState state = chunkState::UNLOADED;
};


class World {
private:
    
    std::unordered_map<glm::ivec3, Chunk, Vec3Hash> WorldData;
    std::queue<glm::ivec3> chunkQueue;
    std::vector<std::future<std::pair<glm::ivec3, Chunk>>> chunkFutures;
    std::unordered_set<glm::ivec3, Vec3Hash> chunkQueueControl;//descobrir porque isso 'e um unordered set e nao um set normal

    std::unordered_map<std::pair<int, int>, int, PairHash> highestChunkY; //melhorar isso

    std::queue<std::pair<int,int>> sunlightQueue;
    std::set<std::pair<int, int>> sunlightQueueControl;

    std::queue<std::pair<int, int>> floodFillQueue;
    std::set<std::pair<int, int>> floodFillQueueControl;

public:

    float sunAngle = 0.0f; // Começa no leste
    float sunSpeed = 5.0f;

public:
    World();

    std::optional<RaycastHit> World::isBlockAir(glm::ivec3 blockPos);

    void World::update(Camera& camera, float deltaTime, unsigned int modelLoc, int& drawCallCount);

    void World::genWorld(Camera& camera, unsigned int modelLoc);

    void World::renderWorld(unsigned int modelLoc, int &drawCallCount);

    void World::tick();

    void World::removeBlock(RaycastHit& hit);

    void World::placeBlock(Camera& camera, RaycastHit& hit, Block blockToPlace);

    void World::castSunlight(Chunk& chunk);

    void World::addToChunkGenQueue(glm::vec3 chunkToAdd);

    void World::addToSunCastQueue(std::pair<int, int> chunkToAdd) {
        sunlightQueue.push(chunkToAdd);
        sunlightQueueControl.insert(chunkToAdd);
    }

    void World::addToLightPropagationQueue(std::pair<int, int> chunkToAdd) {
        floodFillQueue.push(chunkToAdd);
        floodFillQueueControl.insert(chunkToAdd);
    }


    //basicamente inutil, so usa pra GUI, 
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
