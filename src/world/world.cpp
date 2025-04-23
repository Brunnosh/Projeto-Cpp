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

    genWorld(camera, modelLoc);
    tick();

    //light

    Shaders[shaderType::MAIN].use();
    renderWorld(modelLoc, drawCallCount);
    
    
}

void World::tick() {
    // Future entity ticking logic
}



//future make world gen start from player
void World::genWorld(Camera& camera, unsigned int modelLoc) {
    glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(camera.position / float(CHUNKSIZE)));

    short renderDist = camera.renderDist;

    for (int x = -renderDist; x <= renderDist; x++) {
        for (int z = -renderDist; z <= renderDist; z++) {
            for (int y = playerChunkPos.y - renderDist / 2; y <= playerChunkPos.y + renderDist; y++) {
                glm::ivec3 offset(x, y, z);
                glm::ivec3 chunkWorldPos = glm::ivec3(playerChunkPos.x, 0, playerChunkPos.z) + offset;

                auto obj = WorldData.find(chunkWorldPos);
                if (obj == WorldData.end()) {
                    WorldData[chunkWorldPos] = { nullptr, chunkState::LOADING };
                    if (chunkQueueControl.insert(chunkWorldPos).second) {
                        chunkQueue.push(chunkWorldPos);
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

    int chunksPerFrame = 6;
    int generated = 0;
    while (!chunkQueue.empty() && generated < chunksPerFrame) {
        glm::ivec3 pos = chunkQueue.front();
        chunkQueue.pop();

        std::future<std::pair<glm::ivec3, std::shared_ptr<Chunk>>> fut = std::async(std::launch::async, [pos]() -> std::pair<glm::ivec3, std::shared_ptr<Chunk>> {
            auto chunk = std::make_shared<Chunk>(pos);
            World_Gen::generateChunkData(*chunk);
            chunk->isChunkEmpty();
            chunk->needsMeshUpdate = true;
            chunk->needsLightUpdate = true;
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

            WorldData[pos] = std::move(obj);
            chunkFutures.erase(chunkFutures.begin() + i);
            chunkQueueControl.erase(pos);
        }
        else {
            ++i;
        }
    }


}

void World::renderWorld(unsigned int modelLoc, int &drawCallCount) {

    

    for (auto& [pos, obj] : WorldData) {
        if (!obj.chunk || obj.state == chunkState::UNLOADED) continue;

        auto& chunk = *obj.chunk;
        if (chunk.isEmpty) continue;

        if (chunk.needsMeshUpdate) {
            chunk.regenMesh();
            chunk.needsMeshUpdate = false;
            obj.state = chunkState::READY;
        }

        chunk.render(modelLoc);
        drawCallCount++;
    }
}




/*
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
    hit.chunk->isChunkEmpty();
    hit.chunk->needsLightUpdate = true;
    hit.chunk->needsMeshUpdate = true;

    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = hit.chunk->worldPos + offset;
        auto it = WorldData.find(neighborPos);
        if (it != WorldData.end()) {
            it->second.isChunkEmpty();
            it->second.needsLightUpdate = true;
            it->second.needsMeshUpdate = true;
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
        hit.chunk->needsLightUpdate = true;
        hit.chunk->needsMeshUpdate = true;
    }
    else 
    {
        auto it = WorldData.find(newBlockChunkPos);
        if (it == WorldData.end()) {
            std::cerr << "[ERRO] Construindo em chunk não gerado!" << std::endl;
            throw std::runtime_error("Construindo em chunk não gerado!.");
        }

        Chunk* chunk = &it->second;
        std::cout << "Light level do bloco substituido (outro chunk): " << (int)chunk->chunkData[newBlockIndex].getSkyLight() << "\n";
        chunk->chunkData[newBlockIndex] = blockToPlace;
        chunk->isEmpty = false;
        chunk->needsLightUpdate = true;
        chunk->needsMeshUpdate = true;
        
        

    }


    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = newBlockChunkPos + offset;
        auto it = WorldData.find(neighborPos);
        if (it != WorldData.end()) {
            it->second.needsLightUpdate = true;
            it->second.needsMeshUpdate = true;
            
        }
        };

    if (newBlockRelativePos.x == 0)         tryMark(glm::ivec3(-1, 0, 0));
    else if (newBlockRelativePos.x == max)  tryMark(glm::ivec3(1, 0, 0));

    if (newBlockRelativePos.y == 0)         tryMark(glm::ivec3(0, -1, 0));
    else if (newBlockRelativePos.y == max)  tryMark(glm::ivec3(0, 1, 0));

    if (newBlockRelativePos.z == 0)         tryMark(glm::ivec3(0, 0, -1));
    else if (newBlockRelativePos.z == max)  tryMark(glm::ivec3(0, 0, 1));

}


void World::castSunlight(Chunk& chunk) {
    //this chunk is the top most one in its X-Z coordinates, keep going down until solid ground is hit.
    Chunk* currentChunk = &chunk;
    Chunk* chunkAbove = nullptr;


    while (true) {
        currentChunk->needsLightUpdate = false;
        currentChunk->sunlightCalculated = true;
        currentChunk->needsMeshUpdate = true;
        std::cout << "Chunk castSunLight: " << currentChunk->worldPos.x << "," << currentChunk->worldPos.y << "," << currentChunk->worldPos.z << "\n";

        if (currentChunk->isEmpty) {
            for (int i = 0; i < currentChunk->chunkData.size(); i++) {
                currentChunk->chunkData[i].setSkyLight(15);
            }
        }
        else {
            // Chunk com blocos - checar se a luz deve continuar com base no chunk acima
            for (int localX = 0; localX < CHUNKSIZE; ++localX) {
                for (int localZ = 0; localZ < CHUNKSIZE; ++localZ) {

                    bool sunlightContinues = true;
                    for (int localY = CHUNKSIZE - 1; localY >= 0; --localY) {
                        int index = localX * CHUNKSIZE * CHUNKSIZE + localZ * CHUNKSIZE + localY;
                        Block& block = currentChunk->chunkData[index];

                        if (!sunlightContinues) {
                            block.setSkyLight(0);
                            continue;
                        }

                        if (block.getType() == BlockType::AIR) {
                            block.setSkyLight(15);
                        }
                        else {
                            sunlightContinues = false;
                            block.setSkyLight(0);
                        }
                    }

                    // Se chunk acima existe, verifica se ele tem luz no topo dessa coluna
                    if (chunkAbove) {
                        int aboveIndex = localX * CHUNKSIZE * CHUNKSIZE + localZ * CHUNKSIZE + 0; // topo do chunk de baixo = fundo do chunk de cima
                        Block& aboveBlock = chunkAbove->chunkData[aboveIndex];
                        if (aboveBlock.getSkyLight() < 15) {
                            // Se não tem luz vindo de cima, desativa luz nessa coluna
                            sunlightContinues = false;
                        }
                    }
                }
            }
        }
        auto it = WorldData.find({ currentChunk->worldPos.x, currentChunk->worldPos.y - 1, currentChunk->worldPos.z });
        if (it == WorldData.end()) {
            break;
        }
        chunkAbove = currentChunk;
        currentChunk = &it->second;
    }
}
*/