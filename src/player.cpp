#include <player.h>



Player::Player(glm::vec3 position) {
    camera = Camera(glm::vec3(position.x, position.y + 2.0f , position.z));
    playerPos = position;
    camLastX = 0.0f;
    camLastY = 0.0f;
    firstMouse = true;
    menu = false;
}


void Player::movePlayer(CameraMovement direction, float deltaTime) {

    calcPlayerMovement(direction, deltaTime);
    camera.processCamMovement(direction, deltaTime, moveSpeed);
}

void Player::rotateCamera( float xOffset, float yOffset, bool constrainPitch) {

    camera.processMouse(mouseSensitivity, xOffset,  yOffset,  constrainPitch = true);

}


void Player::calcPlayerMovement(CameraMovement direction, float deltaTime) {
    float velocity = moveSpeed * deltaTime;
    glm::vec3 moveDir;

    // Move no plano XZ, mesmo que o front esteja inclinado
    glm::vec3 horizontalFront = glm::normalize(glm::vec3(camera.front.x, 0.0f, camera.front.z));
    switch (direction) {
    case CameraMovement::FORWARD:  moveDir = horizontalFront; break;
    case CameraMovement::BACKWARD: moveDir = -horizontalFront; break;
    case CameraMovement::LEFT:     moveDir = -camera.right; break;
    case CameraMovement::RIGHT:    moveDir = camera.right; break;
    case CameraMovement::UP:       moveDir = glm::vec3(0.0f, 1.0f, 0.0f); break;
    case CameraMovement::DOWN:     moveDir = glm::vec3(0.0f, -1.0f, 0.0f); break;
    }
    playerPos += moveDir * velocity;
}








glm::vec3 Player::getPosition() const {
    return camera.position;
}
