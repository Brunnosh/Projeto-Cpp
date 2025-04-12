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
	glm::vec3 lastPlayerPos; // por enquanto vai ser sempre 0,17,0 (especificado no construtor) , mas quando tiver mundo persistente ler isso do arqivo
	
private:
	

public:
	
	
	World(Camera & camera);

	void update(Camera &camera, float deltaTime, unsigned int modelLoc);
	
	void pushChunkData();


};



#endif 