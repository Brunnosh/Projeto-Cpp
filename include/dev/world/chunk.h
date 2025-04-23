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
	bool isEmpty = false;

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

	bool Chunk::isAirAt(int x, int y, int z, std::vector<Block>* chunkData, std::vector<Block>* nextChunkData);
	
	bool Chunk::isChunkEmpty();

	
};
#endif