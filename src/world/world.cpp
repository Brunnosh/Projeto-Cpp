#include <world.h>
#include <shader.h>
#include <camera.h>
#include <world_gen.h>

float lightWaitTime = 10.0f;
float timer = 0.0f;


World::World() {
    //futuro:
    //ler pos do player salvo, renderizar chunks em torno do player primeiro, depois começar o update.
}

void World::update(Camera & camera, float deltaTime, unsigned int modelLoc, int& drawCallCount) {
    sunAngle += sunSpeed * deltaTime;
    if (sunAngle >= 360.0f)
        sunAngle -= 360.0f;

    float sunRadians = glm::radians(sunAngle + 180.0f);
    glm::vec3 sunDirection = glm::normalize(glm::vec3(cos(sunRadians), sin(sunRadians), 0.0f));
    float sunHeight = sunDirection.y;  // Pega a componente Y da direção do sol
    glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "sunHeight"), sunHeight);
    glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "ambientStrength"), 0.15f);
    glUniform3fv(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "lightDir"), 1, &sunDirection[0]);
    glUniform3fv(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "viewPos"), 1, &camera.position[0]);

    glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "specularStrength"), 0.05f);
    glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "shininess"), 8.0f);

    queueChunks(camera);

    genChunks();
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
            for (int y = playerChunkPos.y - renderDist / 2; y <= playerChunkPos.y + renderDist; y++) {
                glm::ivec3 offset(x, y, z);
                glm::ivec3 chunkWorldPos = glm::ivec3(playerChunkPos.x, 0, playerChunkPos.z) + offset;


                //Future: look in world file first for the chunk, if not, then continue with adding it to queue.


                auto obj = worldData.find(chunkWorldPos);
                if (obj == worldData.end()) {
                    worldData[chunkWorldPos] = { nullptr, chunkState::LOADING };
                    if (chunkGenQueueControl.insert(chunkWorldPos).second) {
                        chunkGenQueue.push(chunkWorldPos);
                    }
                }

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

void World::genChunks() {
    int chunksPerFrame = 6;
    int generated = 0;
    while (!chunkGenQueue.empty() && generated < chunksPerFrame) {
        glm::ivec3 pos = chunkGenQueue.front();
        chunkGenQueue.pop();

        std::future<std::pair<glm::ivec3, std::shared_ptr<Chunk>>> fut = std::async(std::launch::async, [pos]() -> std::pair<glm::ivec3, std::shared_ptr<Chunk>> {
            auto chunk = std::make_shared<Chunk>(pos);
            World_Gen::generateChunkData(*chunk);
            chunk->isChunkEmpty();
            return { pos, chunk };
            });

        chunkFutures.push_back(std::move(fut));
        generated++;
    }

    for (int i = 0; i < chunkFutures.size(); ) {
        auto& fut = chunkFutures[i];
        if (fut.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            auto [pos, chunkPtr] = fut.get();

            chunkObject obj;
            obj.chunk = chunkPtr;
            obj.state = chunkState::GENERATED;

            worldData[pos] = std::move(obj);
            chunkFutures.erase(chunkFutures.begin() + i);
            chunkGenQueueControl.erase(pos);
        }
        else {
            ++i;
        }
    }

}







std::optional<RaycastHit> World::isBlockAir(glm::ivec3 blockPos) {
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



}


void World::removeBlock(RaycastHit& hit) {
    int max = CHUNKSIZE - 1;

    int index = hit.blockRelativePos.x * CHUNKSIZE * CHUNKSIZE + hit.blockRelativePos.z * CHUNKSIZE + hit.blockRelativePos.y;
    hit.chunk->chunkData[index] = Blocks[BlockType::AIR];
    hit.chunk->isChunkEmpty();


    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = hit.chunk->worldPos + offset;
        auto it = worldData.find(neighborPos);
        if (it != worldData.end()) {

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
        std::cout << "Light level do bloco substituido (mesmo chunk): " << (int)hit.chunk->chunkData[newBlockIndex].getSkyLight() << "\n";
        hit.chunk->chunkData[newBlockIndex] = blockToPlace;
        hit.chunk->isEmpty = false;

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

        
        

    }


    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = newBlockChunkPos + offset;
        auto it = worldData.find(neighborPos);
        if (it != worldData.end()) {

            
        }
        };

    if (newBlockRelativePos.x == 0)         tryMark(glm::ivec3(-1, 0, 0));
    else if (newBlockRelativePos.x == max)  tryMark(glm::ivec3(1, 0, 0));

    if (newBlockRelativePos.y == 0)         tryMark(glm::ivec3(0, -1, 0));
    else if (newBlockRelativePos.y == max)  tryMark(glm::ivec3(0, 1, 0));

    if (newBlockRelativePos.z == 0)         tryMark(glm::ivec3(0, 0, -1));
    else if (newBlockRelativePos.z == max)  tryMark(glm::ivec3(0, 0, 1));

}

