#include <world.h>

World::World(Camera& camera,int maxSize, int maxHeight){
	this->maxSize = maxSize;
	this->maxHeight = maxHeight;
	this->lastPlayerPos = glm::vec3(0.0f, 0.0f, 0.0f);


	camera.position = lastPlayerPos;
}


void World::update(Camera& camera,unsigned int modelLoc) {
	glm::vec3 playerPos = camera.getCamPos();

	//ver quais chunks criar, colocar na queue, mandar criar chunk.

}