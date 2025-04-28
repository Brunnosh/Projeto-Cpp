#pragma once
#define WORLD_H

#include <chunk.h>


#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <future>
#include <queue>
#include <set>
#include <stack>


class Renderer;
class Camera;
struct RaycastHit;

enum class chunkState {
    OUTSIDE_RENDER_DISTANCE = -1, //chunk not even instanced in current worldData.
    INVALID, //Chunk does not exist (chunk = nullptr)
    LOADING, // in process of loading/generating chunkData.
    GENERATED, //chunkData is loaded, chunk is not meshed and not prepared.
    QUEUED_LIGHT_UPDATE, //duh
    DIRTY, //light calculated, queued for meshing.
    READY // Chunk is meshed -> will render.

};

struct chunkObject {
    std::shared_ptr<Chunk> chunk = nullptr;
    chunkState state = chunkState::INVALID;
};


class World {
private:
    //std::unordered_map<glm::ivec3, chunkObject, Vec3Hash> worldData; // chunks stored in memory, once removed from render distance store here, then after some time store to disk then remove from memory
    std::unordered_map<glm::ivec3, chunkObject, Vec3Hash> worldData; //basically chunks inside players render distance
    std::queue<glm::ivec3> chunkGenQueue;
    std::unordered_set<glm::ivec3, Vec3Hash> chunkGenQueueControl;
    std::vector<std::future<std::pair<glm::ivec3, std::shared_ptr<Chunk>>>> chunkFutures;
    

    std::unordered_map<std::pair<int, int>, int, PairHash> highestChunkY; 
    std::stack<glm::ivec3> lightUpdateStack;

public:

    float sunAngle = 0.0f; 
    float sunSpeed = 5.0f;

public:
    World();

    void World::queueChunks(Camera& camera);

    void World::genChunks(Renderer& worldRenderer);

    void World::removeFarChunks(Camera& camera);

    void World::update(Camera& camera, float deltaTime, Renderer & worldRenderer);

    std::optional<RaycastHit> World::returnRayCastHit(glm::ivec3 blockPos);

    bool World::isBlockAir(const glm::ivec3 &chunkPos, int x, int y, int z);

    void World::tick();

    void World::removeBlock(RaycastHit& hit, Renderer& worldRenderer);

    void World::placeBlock(Camera& camera, RaycastHit& hit, Block blockToPlace, Renderer & worldRenderer);

    chunkState World::getChunkState(glm::ivec3 pos);

    void World::markForUpdate(chunkObject & obj, Renderer& worldRenderer);

    void World::markColumnToUpdate(const std::pair<int, int>& xz);
  
    int getMaxChunkY(int x, int z) {
        std::pair<int, int> xzKey = { x, z };
        auto it = highestChunkY.find(xzKey);
        if (it != highestChunkY.end() ) {
            return highestChunkY[xzKey];
        }
        
    }
   
    int World::getNumberChunks() {
        return worldData.size();
    }


    std::unordered_map<glm::ivec3, chunkObject, Vec3Hash>& getWorldDataRef() {
        return worldData;
    }


};

