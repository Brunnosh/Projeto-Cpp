



struct Vertex
{
	char X, Y, Z;
	float uvX, uvY;
	char normalX, normalY, normalZ;
	uint8_t lightLevel;

	Vertex(char _posX, char _posY, char _posZ, float _texGridX, float _texGridY, char _normalX, char _normalY, char _normalZ, uint8_t _lightLevel)
		:X(_posX), Y(_posY), Z(_posZ),

		uvX(_texGridX), uvY(_texGridY),

		normalX(_normalX), normalY(_normalY), normalZ(_normalZ),

		lightLevel(_lightLevel) {
	}
};