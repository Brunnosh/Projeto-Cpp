#include <world.h>

float saveTimer = 0.0f;
float saveFrequency = 30.0f;

World::World() {
    initBlockUVs();

    lastPlayerPos = glm::vec3(0.0f, 60.0f, 0.0f); // ler do arquivo
    
    this->player =  Player(lastPlayerPos);


}

void World::update(Player& player, float deltaTime, unsigned int modelLoc) {
    savePlayerPos(deltaTime, player);
    genWorld(player, modelLoc);
}

void World::tick() {
    // Future entity ticking logic
}

void World::savePlayerPos(float deltaTime, Player& player) {
    saveTimer += deltaTime;
    if (saveTimer >= saveFrequency) {
        lastPlayerPos = player.getPosition();
        saveTimer = 0.0f;
    }
}

void World::genWorld(Player& player, unsigned int modelLoc) {
    glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(player.getPosition() / float(CHUNKSIZE)));
    short renderDist = player.getRenderDistance();

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

int World::getNumberChunks() {
    return WorldData.size();
}
