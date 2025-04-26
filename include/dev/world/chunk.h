#ifndef CHUNK_H
#define CHUNK_H


#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <block.h>
#include <hash.h>
#include <voxelVertex.h>

#define CHUNKSIZE 15

class Chunk {
public:
	bool isEmpty = false;

	std::vector<Block> chunkData;
	glm::ivec3 worldPos;

public:

	Chunk::Chunk(glm::ivec3 pos);
	Chunk::~Chunk();

	bool Chunk::isAirAt(int x, int y, int z);
	
	bool Chunk::isChunkEmpty();

	Block Chunk::getBlock(int x, int y, int z);
};
#endif