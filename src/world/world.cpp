#include <world.h>
#include <shader.h>
#include <camera.h>





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

    Shaders[shaderType::MAIN].use();
    renderWorld(modelLoc, drawCallCount);
    
    tick();
}

void World::tick() {
    // Future entity ticking logic
}

void World::calculateChunkLighting(Chunk& chunk) {



    castSunlight(chunk);
    skyLightFloodFill(chunk);

    


}

//Botar os chunks adjacentes para atualizar tbm
void World::skyLightFloodFill(Chunk& chunk) {
    glm::ivec3 chunkPos = chunk.worldPos;
    std::pair<int, int> xzKey = { chunkPos.x, chunkPos.z };
    int maxChunkY = highestChunkY[xzKey];

    for (int localX = 0; localX < CHUNKSIZE; ++localX) {
        for (int localZ = 0; localZ < CHUNKSIZE; ++localZ) {

            bool foundSolid = false;

            for (int yChunk = maxChunkY; yChunk >= chunkPos.y; --yChunk) {
                Chunk* currentChunk = nullptr;
                auto it = WorldData.find({ chunkPos.x, yChunk, chunkPos.z });
                if (it != WorldData.end()) {
                    currentChunk = &(it->second);
                }
                else {
                    continue; // chunk não carregado
                }

                for (int localY = CHUNKSIZE - 1; localY >= 0; --localY) {
                    int index = localX * CHUNKSIZE * CHUNKSIZE + localZ * CHUNKSIZE + localY;
                    Block& block = currentChunk->chunkData[index];

                    if (block.getType() != BlockType::AIR && !foundSolid) {
                        // Obter o bloco acima (pode estar em outro chunk)
                        Block* blockAbove = nullptr;

                        if (localY < CHUNKSIZE - 1) {
                            int aboveIndex = (localY + 1) * CHUNKSIZE * CHUNKSIZE + localZ * CHUNKSIZE + localX;
                            blockAbove = &currentChunk->chunkData[aboveIndex];
                        }
                        else {
                            // Vai buscar o próximo chunk acima
                            auto itAbove = WorldData.find({ chunkPos.x, yChunk + 1, chunkPos.z });
                            if (itAbove != WorldData.end()) {
                                Chunk& chunkAbove = itAbove->second;
                                int aboveIndex = 0 * CHUNKSIZE * CHUNKSIZE + localZ * CHUNKSIZE + localX;
                                blockAbove = &chunkAbove.chunkData[aboveIndex];
                            }
                        }

                        if (blockAbove && blockAbove->getType() == BlockType::AIR) {
                            block.setSkyLight(blockAbove->getSkyLight());
                        }

                        foundSolid = true;
                        break; // esse break é OK, pois só queremos o primeiro sólido
                    }
                }
            }
        }
    }
}


//funcao só bota luz=15 pra todo bloco de ar no chunk até bater num bloco sólido. nao checa se tem acesso ao sol.
void World::castSunlight(Chunk& chunk) {
    if (chunk.isEmpty) {
        for (int i = 0; i < chunk.chunkData.size(); i++) {
            chunk.chunkData[i].setSkyLight(15);
        }
        return;
    }

    glm::ivec3 chunkPos = chunk.worldPos;
    std::pair<int, int> xzKey = { chunkPos.x, chunkPos.z };
    int maxChunkY = highestChunkY[xzKey];

    for (int localX = 0; localX < CHUNKSIZE; ++localX) {
        for (int localZ = 0; localZ < CHUNKSIZE; ++localZ) {

            int worldX = chunkPos.x * CHUNKSIZE + localX;
            int worldZ = chunkPos.z * CHUNKSIZE + localZ;

            // Começa do chunk mais alto e desce
            for (int yChunk = maxChunkY; yChunk >= chunkPos.y; --yChunk) {
                glm::ivec3 currentChunkCoords = glm::ivec3(chunkPos.x, yChunk, chunkPos.z);
                auto it = WorldData.find(currentChunkCoords);

                if (it == WorldData.end()) continue; // Pula se o chunk ainda não existir
                Chunk& targetChunk = it->second;

                for (int localY = CHUNKSIZE - 1; localY >= 0; --localY) {
                    int index = localX * CHUNKSIZE * CHUNKSIZE + localZ * CHUNKSIZE + localY;

                    Block& block = targetChunk.chunkData[index];


                    if (block.getType() == BlockType::AIR) {
                        block.setSkyLight(15); // Iluminação total do céu
                    }
                    else {

                        // Encontrou um bloco sólido, para de propagar a luz
                        break;
                    }
                }
            }
        }
    }
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
        glm::ivec3 pos = chunkQueue.back();
        chunkQueue.pop_back();

        std::future<std::pair<glm::ivec3, Chunk>> fut = std::async(std::launch::async, [pos, this]() -> std::pair<glm::ivec3, Chunk> {
            Chunk chunk(pos);
            chunk.needsMeshUpdate = true;
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
            chunkRequested.erase(pos);
        }
        else {
            ++i;
        }
    }


}

void World::renderWorld(unsigned int modelLoc, int &drawCallCount) {

    

    for (auto& [pos, chunk] : WorldData) {
        chunk.isChunkEmpty();
        if (chunk.isEmpty) { castSunlight(chunk); continue; }
        if (chunk.needsMeshUpdate) {
            
            calculateChunkLighting(chunk);
            chunk.regenMesh(WorldData, highestChunkY);
            chunk.needsMeshUpdate = false;
        }
        
        chunk.render(modelLoc);
        drawCallCount++;
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
    hit.chunk->needsMeshUpdate = true;

    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = hit.chunk->worldPos + offset;
        auto it = WorldData.find(neighborPos);
        if (it != WorldData.end()) {
            it->second.needsMeshUpdate = true;
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
        chunk->chunkData[newBlockIndex] = blockToPlace;
        chunk->needsMeshUpdate = true;
        
        

    }


    auto tryMark = [&](glm::ivec3 offset) {
        auto neighborPos = newBlockChunkPos + offset;
        auto it = WorldData.find(neighborPos);
        if (it != WorldData.end()) {
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

