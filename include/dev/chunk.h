#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <block.h>
#include <glad/glad.h>
#include <glm/glm.hpp>


#define CHUNKSIZE 15


struct RaycastHit;

struct Vec3Hash {
	std::size_t operator()(const glm::ivec3& v) const {
		std::size_t h1 = std::hash<int>()(v.x);
		std::size_t h2 = std::hash<int>()(v.y);
		std::size_t h3 = std::hash<int>()(v.z);
		return h1 ^ (h2 << 1) ^ (h3 << 2); // Combinação simples
	}
};


struct Vertex
{
	char X, Y, Z;
	float uvX, uvY;

	Vertex(char _posX, char _posY, char _posZ, float _texGridX, float _texGridY)
		: X(_posX), Y(_posY), Z(_posZ), uvX(_texGridX), uvY(_texGridY) {
	}
};

class Chunk {
public:
	bool generated;
	bool ready;
	std::vector<Block> chunkData; 
	glm::ivec3 worldPos; // ivec -> chunk pos vai ser em incrementos de 15, (0,0) (15,15,15) (30,30,30).. etc

private:
	unsigned int VBO, VAO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;



public:
	Chunk(glm::ivec3 pos); 
	~Chunk() {
		if (ready) {
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
			glDeleteVertexArrays(1, &VAO);
		}
		
	}


	bool isAirAt(int x, int y, int z, std::vector<Block>* chunkData, std::vector<Block>* nextChunkData);
	void genChunkFaces(std::unordered_map<glm::ivec3, Chunk, Vec3Hash>& WorldData);
	void render(unsigned int modelLoc);
	std::vector<Block> populateChunk(glm::ivec3 chunkCoords);

	void regenMesh(std::unordered_map<glm::ivec3, Chunk, Vec3Hash>& WorldData) {
		this->vertices.clear();
		this->indices.clear();
		this->ready = false;
		this->generated = false;
		this->genChunkFaces(WorldData);
	}

	void placeBlock();
};
#endif