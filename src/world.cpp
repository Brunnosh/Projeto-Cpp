#include <world.h>

World::World(float maxSize, float maxHeight): maxSize(maxSize), maxHeight(maxHeight) {

}


void World::update(Camera& camera,unsigned int modelLoc) {
	glm::vec3 playerPos = camera.getCamPos();

	//ver quais chunks criar, colocar na queue, mandar criar chunk.

}