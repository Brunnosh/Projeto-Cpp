#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chunk.h>
#include <camera.h>
#include <future>


struct Vec3Hash {
	std::size_t operator()(const glm::ivec3& v) const {
		std::size_t h1 = std::hash<int>()(v.x);
		std::size_t h2 = std::hash<int>()(v.y);
		std::size_t h3 = std::hash<int>()(v.z);
		return h1 ^ (h2 << 1) ^ (h3 << 2); // Combinação simples
	}
};


class World {
private:
	std::unordered_map<glm::ivec3, Chunk, Vec3Hash> WorldData; //usar unordered map (Chunk e WorldPos, usando hash table)
	

public:
	std::vector<std::future<std::pair<glm::ivec3, Chunk>>> chunkFutures;
	std::vector<glm::ivec3> chunkQueue;
	std::unordered_set<glm::ivec3, Vec3Hash> chunkRequested;
	glm::vec3 lastPlayerPos; // por enquanto vai ser sempre 0,17,0 (especificado no construtor) , mas quando tiver mundo persistente ler isso do arqivo
	
private:
	

public:
	
	
	World(Camera & camera);

	void update(Camera &camera, float deltaTime, unsigned int modelLoc);
	
	//std::vector<Chunk>& getWorldData() { return this->WorldData; }

	void pushChunkData();


};



#endif 