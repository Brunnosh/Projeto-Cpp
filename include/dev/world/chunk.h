#ifndef CHUNK_H
#define CHUNK_H


#include <vector>
#include <block.h>
#include <glad/glad.h>
#include <glm/glm.hpp>


#define CHUNKSIZE 15

struct chunkBuffers {
	GLuint VBO = 0, VAO = 0, EBO = 0;
};


struct Vec3Hash {
	std::size_t operator()(const glm::ivec3& v) const {
		std::size_t h1 = std::hash<int>()(v.x);
		std::size_t h2 = std::hash<int>()(v.y);
		std::size_t h3 = std::hash<int>()(v.z);
		return h1 ^ (h2 << 1) ^ (h3 << 2); // Combinação simples
	}
};

struct PairHash {
	std::size_t operator()(const std::pair<int, int>& p) const {
		return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
	}
};

struct Vertex
{
	char X, Y, Z;
	float uvX, uvY;
	char normalX, normalY, normalZ;
	uint8_t lightLevel;

	Vertex(char _posX, char _posY, char _posZ, float _texGridX, float _texGridY, char _normalX, char _normalY, char _normalZ, uint8_t _lightLevel)
		: X(_posX), Y(_posY), Z(_posZ), 
		uvX(_texGridX), uvY(_texGridY), 
		normalX(_normalX), normalY(_normalY), normalZ(_normalZ),
		lightLevel(_lightLevel){
	}
};

class Chunk {
public:
	bool generated = false;
	bool ready = false;
	bool isEmpty = false;
	bool needsMeshUpdate;
	bool needsLightUpdate;
	std::vector<Block> chunkData;
	glm::ivec3 worldPos;

private:
	chunkBuffers buffers[2];  // double buffer
	char activeBuffer = 0;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;



public:
	Chunk() {}

	Chunk::Chunk(glm::ivec3 pos) {
		worldPos = pos;
		generated = false;
		ready = false;
		chunkData = populateChunk(pos);
		isChunkEmpty();

	}

	~Chunk() {
		for (int i = 0; i < 2; ++i) {
			if (buffers[i].VAO != 0) {
				glDeleteBuffers(1, &buffers[i].VBO);
				glDeleteBuffers(1, &buffers[i].EBO);
				glDeleteVertexArrays(1, &buffers[i].VAO);
			}
		}
	}


	bool isAirAt(int x, int y, int z, std::vector<Block>* chunkData, std::vector<Block>* nextChunkData);
	void genChunkFaces(std::unordered_map<glm::ivec3, Chunk, Vec3Hash>& WorldData);
	void render(unsigned int modelLoc);
	std::vector<Block> populateChunk(glm::ivec3 chunkCoords);
	void addVertxInfo(FACE face, char x, char y, char z, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int& currentVertex, Block storedBlock);
	void regenMesh(std::unordered_map<glm::ivec3, Chunk, Vec3Hash>& WorldData, std::unordered_map<std::pair<int, int>, int, PairHash>& highestChunkY);
	void calculateChunkLighting(std::unordered_map<glm::ivec3, Chunk, Vec3Hash>& WorldData, std::unordered_map<std::pair<int, int>, int, PairHash> &highestChunkY);

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