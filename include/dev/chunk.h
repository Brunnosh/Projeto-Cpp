#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <block.h>

#define CHUNKSIZE 15


struct Vertex
{
	uint8_t X,Y,Z;
	uint8_t uvX, uvY;

	Vertex(float _posX, float _posY, float _posZ, float _texGridX, float _texGridY)
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
	bool created;
	bool ready;
	std::vector<unsigned short> chunkData; //

private:
	unsigned int vob, vao, ebo;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;



public:
	Chunk(bool created, bool ready): created(false), ready(false){} // AFTER: ADD REAL WORLD POS AND MODEL MATRIX TRANSFORM TO REAL WORLD
	~Chunk();

	void genChunk();
	void render();

};
#endif