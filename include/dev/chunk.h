#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <block.h>
#include <glad/glad.h>

#define CHUNKSIZE 15


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
	std::vector<Block> chunkData; //

private:
	unsigned int VBO, VAO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;



public:
	Chunk(){
		VAO, VBO, EBO = 0;
		generated = false;
		ready = false;



		
	} // AFTER: ADD REAL WORLD POS AND MODEL MATRIX TRANSFORM TO REAL WORLD
	~Chunk() {
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
	}


	bool isAirAt(int x, int y, int z, std::vector<Block>* chunkData);
	void genChunk();
	void render(unsigned int modelLoc);

};
#endif