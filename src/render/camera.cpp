#include <camera.h>
#include <world.h>



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

void Camera::raycast(const std::function<std::optional<RaycastHit>(glm::ivec3)>& isBlockAir) const
{
    glm::vec3 dir = glm::normalize(front);
    glm::ivec3 lastBlockPos = glm::floor(position);

    raycastInfo = std::nullopt;  // Limpa antes de começar

    for (float t = 0.0f; t <= cameraReach; t += raycastStep) {
        glm::vec3 probe = position + dir * t;
        glm::ivec3 blockPos = glm::floor(probe);

        if (blockPos != lastBlockPos) {
            auto result = isBlockAir(blockPos);
            if (result.has_value()) {
                RaycastHit hit = result.value();

                glm::ivec3 delta = blockPos - lastBlockPos;
                if (delta == glm::ivec3(0, 1, 0))       hit.blockFace = FACE::BOTTOM;
                else if (delta == glm::ivec3(0, -1, 0)) hit.blockFace = FACE::TOP;
                else if (delta == glm::ivec3(1, 0, 0))  hit.blockFace = FACE::WEST;
                else if (delta == glm::ivec3(-1, 0, 0)) hit.blockFace = FACE::EAST;
                else if (delta == glm::ivec3(0, 0, 1))  hit.blockFace = FACE::NORTH;
                else if (delta == glm::ivec3(0, 0, -1)) hit.blockFace = FACE::SOUTH;

                raycastInfo = hit;
                return;
            }
            lastBlockPos = blockPos;
        }
    }
}

void Camera::update(World& world) {
    //Collision detection




    //Physics calculation







    //Calc raycast
    raycast([&](glm::ivec3 pos) {
        return world.isBlockAir(pos);
        });
}