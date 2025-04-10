#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <chunk.h>
#include <camera.h>


class World {
private:
	std::vector<Chunk> WorldData; //usar unordered map (Chunk e WorldPos, usando hash table)
	float maxSize;
	float maxHeight;

public:
	std::vector<glm::ivec3> chunkQueue;
	glm::vec3 lastPlayerPos; // por enquanto vai ser sempre 0,0 (especificado no construtor) , mas quando tiver mundo persistente ler isso do arqivo

private:
	World();

public:
	// update chama checkplayer pos, se estiver num chunk não gerado, gerar e renderizar chunk.
	
	World(Camera & camera, int maxSize, int maxHeight);

	void update(Camera &camera, unsigned int modelLoc);
	
	void pushChunkData();


};



#endif 