#pragma once
#define WORLD_H

#include <chunk.h>
#include <renderer.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
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
    Renderer worldRenderer;
    std::unordered_map<glm::ivec3, chunkObject, Vec3Hash> worldData;
    std::queue<glm::ivec3> chunkGenQueue;
    std::unordered_set<glm::ivec3, Vec3Hash> chunkGenQueueControl;
    std::vector<std::future<std::pair<glm::ivec3, std::shared_ptr<Chunk>>>> chunkFutures;
    

    std::unordered_map<std::pair<int, int>, int, PairHash> highestChunkY; //melhorar isso

    //Queue mesh recalculation.
    //Queue light calculation

public:

    float sunAngle = 0.0f; // Começa no leste
    float sunSpeed = 5.0f;

public:
    World();

    void World::queueChunks(Camera& camera);

    void World::genChunks();

    void World::update(Camera& camera, float deltaTime, unsigned int modelLoc, int& drawCallCount);

    std::optional<RaycastHit> World::isBlockAir(glm::ivec3 blockPos);

    


    void World::tick();

    //void World::removeBlock(RaycastHit& hit);

    //void World::placeBlock(Camera& camera, RaycastHit& hit, Block blockToPlace);




    //basicamente inutil, so usa pra GUI, 
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





};

