#include <world.h>

float saveTimer = 0.0f;
float saveFrequency = 30.0f;

World::World() {
    
}

void World::update(Camera & camera, float deltaTime, unsigned int modelLoc) {
    
    genWorld(camera, modelLoc);
}

void World::tick() {
    // Future entity ticking logic
}


void World::genWorld(Camera& camera, unsigned int modelLoc) {
    glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(camera.position / float(CHUNKSIZE)));
    short renderDist = camera.renderDist;

    for (int x = -renderDist; x <= renderDist; x++) {
        for (int z = -renderDist; z <= renderDist; z++) {
            for (int y = playerChunkPos.y - renderDist / 2; y <= playerChunkPos.y + renderDist; y++) {
                glm::ivec3 offset(x, y, z);
                glm::ivec3 chunkWorldPos = glm::ivec3(playerChunkPos.x, 0, playerChunkPos.z) + offset;

                bool inWorld = WorldData.find(chunkWorldPos) != WorldData.end();
                bool alreadyQueued = chunkRequested.find(chunkWorldPos) != chunkRequested.end();

                if (!inWorld && !alreadyQueued) {
                    chunkQueue.push_back(chunkWorldPos);
                    chunkRequested.insert(chunkWorldPos);
                }
            }
        }
    }

    int chunksPerFrame = 4;
    int generated = 0;
    while (!chunkQueue.empty() && generated < chunksPerFrame) {
        glm::ivec3 pos = chunkQueue.back();
        chunkQueue.pop_back();

        std::future<std::pair<glm::ivec3, Chunk>> fut = std::async(std::launch::async, [pos, this]() -> std::pair<glm::ivec3, Chunk> {
            Chunk chunk(pos);
            chunk.genChunkFaces(this->WorldData);
            return { pos, std::move(chunk) };
            });

        chunkFutures.push_back(std::move(fut));
        generated++;
    }

    for (int i = 0; i < chunkFutures.size(); ) {
        auto& fut = chunkFutures[i];
        if (fut.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            auto [pos, chunk] = fut.get();
            WorldData.emplace(pos, std::move(chunk));
            chunkFutures.erase(chunkFutures.begin() + i);
        }
        else {
            ++i;
        }
    }

    for (auto& [pos, chunk] : WorldData) {
        chunk.render(modelLoc);
    }
}



bool World::isBlockAir(glm::ivec3 blockPos) {
    glm::ivec3 blockChunkPos = glm::ivec3(glm::floor(glm::vec3(blockPos) / float(CHUNKSIZE)));
    glm::ivec3 blockOffset = blockPos - blockChunkPos * CHUNKSIZE;
    int blockindex = blockOffset.x * CHUNKSIZE * CHUNKSIZE + blockOffset.z * CHUNKSIZE + blockOffset.y;

    

    auto it = WorldData.find(blockChunkPos);
    if (it != WorldData.end()) {
        std::vector<Block>& chunkdata  = it->second.chunkData;

        if (chunkdata[blockindex].getType() != BlockType::AIR) {
            return true;
        }

    }

    
    return false;
    //chunk criado em X - Z - Y 

}