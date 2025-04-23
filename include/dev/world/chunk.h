#ifndef CHUNK_H
#define CHUNK_H


#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <block.h>
#include <hash.h>
#include <voxelVertex.h>

#define CHUNKSIZE 15

struct chunkBuffers {
	GLuint VBO = 0, VAO = 0, EBO = 0;
};





class Chunk {
public:
	bool generated = false;
	bool ready = false;
	bool isEmpty = false;

	bool needsMeshUpdate;
	bool needsLightUpdate;

	bool sunlightCalculated = false;
	

	std::vector<Block> chunkData;
	glm::ivec3 worldPos;

private:
	chunkBuffers buffers[2];  // double buffer
	char activeBuffer = 0;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;



public:

	Chunk::Chunk(glm::ivec3 pos);

	Chunk::~Chunk();


	bool isAirAt(int x, int y, int z, std::vector<Block>* chunkData, std::vector<Block>* nextChunkData);
	void genChunkFaces();
	void render(unsigned int modelLoc);
	std::vector<Block> populateChunk(glm::ivec3 chunkCoords);
	void addVertxInfo(FACE face, char x, char y, char z, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int& currentVertex, Block storedBlock);
	void regenMesh();


	bool isChunkEmpty() {
		
		
		for (const Block& block : chunkData) {
			if (block.getType() != BlockType::AIR) {
				isEmpty = false;
				return false;
			}
		}
		isEmpty = true;
		return true;
		
	}

	
};
#endif