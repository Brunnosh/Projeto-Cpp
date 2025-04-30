#include <world.h>
#include <shader.h>
#include <camera.h>
#include <world_gen.h>
#include <renderer.h>
#include <lighting.h>

float lightWaitTime = 10.0f;
float timer = 0.0f;


World::World() {
    //futuro:
    //ler pos do player salvo, renderizar chunks em torno do player primeiro, depois começar o update.
}

void World::update(Camera & camera, float deltaTime, Renderer & worldRenderer, ThreadPool& pool) {
    
    Lighting::processPendingColumns(*this, worldRenderer,  pool);

}

void World::tick() {
    // Future entity ticking logic
}



//future make world gen start from player
void World::queueChunks(Camera& camera) {
    glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(camera.position / float(CHUNKSIZE)));
  
    short renderDist = camera.renderDist;

    for (int x = -renderDist; x <= renderDist; x++) {
        for (int z = -renderDist; z <= renderDist; z++) {
            for (int y = playerChunkPos.y - renderDist/2; y <= playerChunkPos.y + renderDist; y++) {
                glm::ivec3 offset(x, y, z);
                glm::ivec3 chunkWorldPos = glm::ivec3(playerChunkPos.x, 0, playerChunkPos.z) + offset;


                //Future: look in world file first for the chunk, if not, then continue with adding it to queue.

                //Process chunks retrieved from file first

                auto obj = worldData.find(chunkWorldPos);
                if (obj == worldData.end()) {
                    worldData[chunkWorldPos] = { nullptr, chunkState::LOADING };
                    if (chunkGenQueueControl.insert(chunkWorldPos).second) {

                        chunkGenQueue.push(chunkWorldPos);
                    }
                }
                //Lighting::queueColumnForLightingUpdate(chunkWorldPos.x, chunkWorldPos.z);
                std::pair<int, int> xzKey = { chunkWorldPos.x, chunkWorldPos.z };
                auto it = highestChunkY.find(xzKey);
                if (it == highestChunkY.end() || chunkWorldPos.y > it->second) {
                    highestChunkY[xzKey] = chunkWorldPos.y;
                }

                
            }
        }
    }

    removeFarChunks(camera);

}

void World::genChunks(Renderer& worldRenderer, ThreadPool& pool) {
    int chunksPerFrame = 6;
    int generated = 0;

    while (!chunkGenQueue.empty() && generated < chunksPerFrame) {
        glm::ivec3 pos = chunkGenQueue.front();
        chunkGenQueue.pop();
        chunkGenQueueControl.erase(pos);

        pool.enqueue([this, pos]() {
            auto chunk = std::make_shared<Chunk>(pos);
            World_Gen::generateChunkData(*chunk);
            chunk->isChunkEmpty();

            std::lock_guard<std::mutex> lock(worldMutex);
            chunkObject obj;
            obj.chunk = chunk;
            obj.state = chunkState::QUEUED_LIGHT_UPDATE;
            worldData[pos] = std::move(obj);
            Lighting::queueColumnForLightingUpdate(pos.x, pos.z);
            });

        generated++;
    }
}






std::optional<RaycastHit> World::returnRayCastHit(glm::ivec3 blockPos) {
    glm::ivec3 blockChunkPos = glm::ivec3(glm::floor(glm::vec3(blockPos) / float(CHUNKSIZE)));
    glm::ivec3 blockOffset = blockPos - blockChunkPos * CHUNKSIZE;
    int blockindex = blockOffset.x * CHUNKSIZE * CHUNKSIZE + blockOffset.z * CHUNKSIZE + blockOffset.y;

    auto it = worldData.find(blockChunkPos);
    if (it == worldData.end() || !it->second.chunk) return std::nullopt;

    std::shared_ptr<Chunk> chunk = it->second.chunk;

    if (chunk->chunkData[blockindex].getType() == BlockType::AIR)
        return std::nullopt;

    Block* selectedBlock = &chunk->chunkData[blockindex];

    return RaycastHit{ chunk.get(), blockOffset, blockPos, selectedBlock };
}

void World::removeFarChunks(Camera& camera) {
    //remove from worldData and renderer->chunkRenderMap


}


void World::removeBlock(RaycastHit& hit, Renderer& worldRenderer) {
    int max = CHUNKSIZE - 1;

    int index = hit.blockRelativePos.x * CHUNKSIZE * CHUNKSIZE + hit.blockRelativePos.z * CHUNKSIZE + hit.blockRelativePos.y;

    auto it = worldData.find(hit.chunk->worldPos);

    hit.chunk->chunkData[index] = Blocks[BlockType::AIR];
    hit.chunk->isChunkEmpty();

    markForUpdate(it->second, worldRenderer);


    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = hit.chunk->worldPos + offset;
        auto it = worldData.find(neighborPos);
        if (it != worldData.end()) {
            it->second.chunk->isChunkEmpty();
            markForUpdate(it->second, worldRenderer);
        }
        };

    if (hit.blockRelativePos.x == 0)         tryMark(glm::ivec3(-1, 0, 0));
    else if (hit.blockRelativePos.x == max)  tryMark(glm::ivec3(1, 0, 0));

    if (hit.blockRelativePos.y == 0)         tryMark(glm::ivec3(0, -1, 0));
    else if (hit.blockRelativePos.y == max)  tryMark(glm::ivec3(0, 1, 0));

    if (hit.blockRelativePos.z == 0)         tryMark(glm::ivec3(0, 0, -1));
    else if (hit.blockRelativePos.z == max)  tryMark(glm::ivec3(0, 0, 1));

    //

}

void World::placeBlock(Camera& camera, RaycastHit & hit, Block blockToPlace, Renderer& worldRenderer) {
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

        auto it = worldData.find(hit.chunk->worldPos);

        std::cout << "Light level do bloco substituido (mesmo chunk): " << (int)hit.chunk->chunkData[newBlockIndex].getSkyLight() << "\n";
        hit.chunk->chunkData[newBlockIndex] = blockToPlace;
        hit.chunk->isEmpty = false;
        markForUpdate(it->second, worldRenderer);
    }
    else 
    {
        auto it = worldData.find(newBlockChunkPos);
        if (it == worldData.end()) {
            std::cerr << "[ERRO] Construindo em chunk não gerado!" << std::endl;
            throw std::runtime_error("Construindo em chunk não gerado!.");
        }

        std::shared_ptr<Chunk> chunk = it->second.chunk;
        std::cout << "Light level do bloco substituido (outro chunk): " << (int)chunk->chunkData[newBlockIndex].getSkyLight() << "\n";
        chunk->chunkData[newBlockIndex] = blockToPlace;
        chunk->isEmpty = false;
        markForUpdate(it->second, worldRenderer);
        

    }


    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = newBlockChunkPos + offset;
        auto it = worldData.find(neighborPos);
        if (it != worldData.end()) {
            markForUpdate(it->second, worldRenderer);
        }
        };

    if (newBlockRelativePos.x == 0)         tryMark(glm::ivec3(-1, 0, 0));
    else if (newBlockRelativePos.x == max)  tryMark(glm::ivec3(1, 0, 0));

    if (newBlockRelativePos.y == 0)         tryMark(glm::ivec3(0, -1, 0));
    else if (newBlockRelativePos.y == max)  tryMark(glm::ivec3(0, 1, 0));

    if (newBlockRelativePos.z == 0)         tryMark(glm::ivec3(0, 0, -1));
    else if (newBlockRelativePos.z == max)  tryMark(glm::ivec3(0, 0, 1));

}

bool World::isBlockAir(const glm::ivec3 &chunkPos, int x, int y, int z) {
 
   


    auto it = worldData.find(chunkPos);
    if (it == worldData.end()) return true;

     return it->second.chunk->isAirAt(x,y,z);

}

chunkState World::getChunkState(glm::ivec3 pos) {
    auto it = worldData.find(pos);
    if (it == worldData.end()) return chunkState::OUTSIDE_RENDER_DISTANCE;

    return it->second.state;

}

void World::markColumnToUpdate(const std::pair<int, int>& xz) {
    int highestY = getMaxChunkY(xz.first, xz.second);

    int chunkY = highestY;
    while (true) {
        glm::ivec3 chunkPos = { xz.first, chunkY, xz.second };



        auto it = worldData.find(chunkPos);
        if (it == worldData.end()) {
            // Se não existe chunk, está tudo bem, terminamos aqui
            return;
        }

        it->second.state = chunkState::QUEUED_LIGHT_UPDATE;

        chunkY--;
    }
}

void World::markForUpdate(chunkObject& obj, Renderer & worldRenderer) {
    obj.state = chunkState::QUEUED_LIGHT_UPDATE;

    markColumnToUpdate(std::pair(obj.chunk->worldPos.x, obj.chunk->worldPos.z));

    Lighting::queueColumnForLightingUpdate(obj.chunk->worldPos.x, obj.chunk->worldPos.z);
}

