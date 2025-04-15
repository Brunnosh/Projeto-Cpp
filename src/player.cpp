#include <player.h>

Player::Player() {
    camera = Camera(glm::vec3(0.0f, 62.0f, 0.0f));
    camLastX = 0.0f;
    camLastY = 0.0f;
    firstMouse = true;
    menu = false;
}
glm::vec3 Player::getPosition() const {
    return camera.position;
}
