#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <block.h>
#include <glad/glad.h>

#define CHUNKSIZE 15


struct Vertex
{
	uint8_t X,Y,Z;
	uint8_t uvX, uvY;

	Vertex(uint8_t _posX, uint8_t _posY, uint8_t _posZ, uint8_t _texGridX, uint8_t _texGridY)
	{
		X = _posX;
		Y = _posY;
		Z = _posZ;

		uvX = _texGridX;
		uvY = _texGridY;
	}
};

class Chunk {
public:
	bool generated;
	bool ready;
	std::vector<unsigned short> chunkData; //

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

	void genChunk();
	void render(unsigned int modelLoc);

};
#endif