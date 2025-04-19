#include <world.h>

float saveTimer = 0.0f;
float saveFrequency = 30.0f;

World::World() {
    //futuro:
    //ler pos do player salvo, renderizar chunks em torno do player primeiro, depois começar o update.
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
            chunk.dirty = true;
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
        if (chunk.dirty) {
            chunk.regenMesh(WorldData);
            chunk.dirty = false;
        }
        chunk.render(modelLoc);
    }
}


std::optional<RaycastHit> World::isBlockAir(glm::ivec3 blockPos) {
    glm::ivec3 blockChunkPos = glm::ivec3(glm::floor(glm::vec3(blockPos) / float(CHUNKSIZE)));
    glm::ivec3 blockOffset = blockPos - blockChunkPos * CHUNKSIZE;
    int blockindex = blockOffset.x * CHUNKSIZE * CHUNKSIZE + blockOffset.z * CHUNKSIZE + blockOffset.y;

    auto it = WorldData.find(blockChunkPos);
    if (it == WorldData.end()) return std::nullopt;


    Chunk* chunk = &it->second;
    Block* selectedBlock = &chunk->chunkData[blockindex];
    

    
    if (chunk->chunkData[blockindex].getType() == BlockType::AIR)
        return std::nullopt;

    return RaycastHit{ chunk, blockOffset, blockPos, selectedBlock };


}

void World::removeBlock(RaycastHit& hit) {
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

void World::placeBlock(Camera& camera, RaycastHit & hit, Block blockToPlace) {
    int max = CHUNKSIZE - 1;
    glm::ivec3 newBlockPos;
    FACE face = hit.blockFace;

    switch (face) {
    case TOP:
        newBlockPos = glm::ivec3(hit.blockWorldPos.x, hit.blockWorldPos.y + 1, hit.blockWorldPos.z);
        break;
    case BOTTOM:
        newBlockPos = glm::ivec3(hit.blockWorldPos.x, hit.blockWorldPos.y - 1, hit.blockWorldPos.z);
        break;
    case NORTH:
        newBlockPos = glm::ivec3(hit.blockWorldPos.x, hit.blockWorldPos.y, hit.blockWorldPos.z - 1);
        break;
    case SOUTH:
        newBlockPos = glm::ivec3(hit.blockWorldPos.x, hit.blockWorldPos.y, hit.blockWorldPos.z + 1);
        break;
    case EAST:
        newBlockPos = glm::ivec3(hit.blockWorldPos.x + 1, hit.blockWorldPos.y, hit.blockWorldPos.z);
        break;
    case WEST:
        newBlockPos = glm::ivec3(hit.blockWorldPos.x - 1, hit.blockWorldPos.y, hit.blockWorldPos.z);
        break;
    }

    if (glm::ivec3(camera.position) == newBlockPos) return;

    glm::ivec3 newBlockChunkPos = glm::ivec3(glm::floor(glm::vec3(newBlockPos) / float(CHUNKSIZE)));
    glm::ivec3 newBlockRelativePos = newBlockPos - newBlockChunkPos * CHUNKSIZE;

    int newBlockIndex = newBlockRelativePos.x * CHUNKSIZE * CHUNKSIZE + newBlockRelativePos.z * CHUNKSIZE + newBlockRelativePos.y;


    //Operação contida dentro do proprio chunk
    if (hit.chunk->worldPos == newBlockChunkPos) {
        hit.chunk->chunkData[newBlockIndex] = blockToPlace;
        hit.chunk->dirty = true;
    }
    else {
        auto it = WorldData.find(newBlockChunkPos);
        if (it == WorldData.end()) {
            std::cerr << "[ERRO] Construindo em chunk não gerado!" << std::endl;
            throw std::runtime_error("Construindo em chunk não gerado!.");
        }

        Chunk* chunk = &it->second;
        chunk->chunkData[newBlockIndex] = blockToPlace;
        chunk->dirty = true;
    }


    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = newBlockChunkPos + offset;
        auto it = WorldData.find(neighborPos);
        if (it != WorldData.end()) {
            it->second.dirty = true;
        }
        };

    if (newBlockRelativePos.x == 0)         tryMark(glm::ivec3(-1, 0, 0));
    else if (newBlockRelativePos.x == max)  tryMark(glm::ivec3(1, 0, 0));

    if (newBlockRelativePos.y == 0)         tryMark(glm::ivec3(0, -1, 0));
    else if (newBlockRelativePos.y == max)  tryMark(glm::ivec3(0, 1, 0));

    if (newBlockRelativePos.z == 0)         tryMark(glm::ivec3(0, 0, -1));
    else if (newBlockRelativePos.z == max)  tryMark(glm::ivec3(0, 0, 1));




}