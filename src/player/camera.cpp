#include <camera.h>
#include <window.h>




void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = moveSpeed * deltaTime;
    glm::vec3 moveDir;

    // Move no plano XZ, mesmo que o front esteja inclinado
    glm::vec3 horizontalFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    switch (direction) {
    case CameraMovement::FORWARD:  moveDir = horizontalFront; break;
    case CameraMovement::BACKWARD: moveDir = -horizontalFront; break;
    case CameraMovement::LEFT:     moveDir = -right; break;
    case CameraMovement::RIGHT:    moveDir = right; break;
    case CameraMovement::UP:       moveDir = glm::vec3(0.0f, 1.0f, 0.0f); break;
    case CameraMovement::DOWN:     moveDir = glm::vec3(0.0f, -1.0f, 0.0f); break;
    }
    position += moveDir * velocity;
}


void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    updateVectors();

}

void Camera::processMouseScroll(float yOffset) {
    fov -= yOffset;
    fov = glm::clamp(fov, 1.0f, 75.0f);

}

