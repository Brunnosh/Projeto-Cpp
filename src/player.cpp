#include <player.h>

Player::Player() {
    std::cout << "constutor default player chamado0" << " ";
    camera = Camera(glm::vec3(0.0f, 0.0f,0.0f));
    playerPos = glm::vec3(0.0f, 0.0f, 0.0f);
    camLastX = 0.0f;
    camLastY = 0.0f;
    firstMouse = true;
    menu = false;
}

Player::Player(glm::vec3 position) {
    camera = Camera(glm::vec3(position.x, position.y + 2.0f , position.z));
    playerPos = position;
    camLastX = 0.0f;
    camLastY = 0.0f;
    firstMouse = true;
    menu = false;
}


void Player::movePlayer() {


}

void Player::rotateCamera() {

}











glm::vec3 Player::getPosition() const {
    return camera.position;
}
