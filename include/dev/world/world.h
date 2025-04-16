#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chunk.h>
#include <camera.h>
#include <future>


class World {
private:
    std::unordered_map<glm::ivec3, Chunk, Vec3Hash> WorldData;

public:
    std::vector<std::future<std::pair<glm::ivec3, Chunk>>> chunkFutures;
    std::vector<glm::ivec3> chunkQueue;
    std::unordered_set<glm::ivec3, Vec3Hash> chunkRequested;




public:
    World();

    void update(Camera& camera, float deltaTime, unsigned int modelLoc);

    void genWorld(Camera& camera, unsigned int modelLoc);
    void tick();
    int getNumberChunks() {
        return WorldData.size();
    }

    std::optional<RaycastHit> isBlockAir(glm::ivec3 blockPos);

    void removeBlock(RaycastHit& hit) {
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

    void placeBlock(RaycastHit& hit, Block blockToPlace ) {
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
   
};

#endif
