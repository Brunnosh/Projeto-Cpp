#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chunk.h>
#include <future>
#include <player.h>

class World {
private:
    std::unordered_map<glm::ivec3, Chunk, Vec3Hash> WorldData;

public:
    std::vector<std::future<std::pair<glm::ivec3, Chunk>>> chunkFutures;
    std::vector<glm::ivec3> chunkQueue;
    std::unordered_set<glm::ivec3, Vec3Hash> chunkRequested;
    glm::vec3 lastPlayerPos;

    Player player;

public:
    World();

    void update(Player& player, float deltaTime, unsigned int modelLoc);
    void savePlayerPos(float deltaTime, Player& player);
    void genWorld(Player& player, unsigned int modelLoc);
    void tick();
    int getNumberChunks();

    Player& getPlayer() { return player; }
};

#endif
