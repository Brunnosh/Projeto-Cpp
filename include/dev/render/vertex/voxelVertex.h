#pragma once
#include <glm/detail/qualifier.hpp>



struct Vertex
{
	char X, Y, Z;
	float uvX, uvY;
	char normalX, normalY, normalZ;
	uint8_t lightByte = 0;


	Vertex(char _posX, char _posY, char _posZ, float _texGridX, float _texGridY, char _normalX, char _normalY, char _normalZ, uint8_t _skyLight, uint8_t _blockLight)
		:X(_posX), Y(_posY), Z(_posZ),

		uvX(_texGridX), uvY(_texGridY),

		normalX(_normalX), normalY(_normalY), normalZ(_normalZ), lightByte(0)
	{

		lightByte |= _skyLight;
		lightByte |= _blockLight << 4;


	}
};