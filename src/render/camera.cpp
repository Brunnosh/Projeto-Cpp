#include <camera.h>
#include <world.h>

void Camera::update(World& world) {
    raycast([&](glm::ivec3 pos) {
        return world.isBlockAir(pos);
        });
}