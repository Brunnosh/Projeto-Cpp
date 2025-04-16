#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <optional>
#include <functional>
#include <chunk.h>

struct RaycastHit {
    Chunk* chunk;
    glm::ivec3 blockRelativePos;
    glm::ivec3 blockWorldPos;
    FACE blockFace;
};

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    // Configurações principais
    glm::vec3 position;
    float yaw;
    float pitch;
    float fov;
    short renderDist = 8;
    float moveSpeed = 10.0f;
    float mouseSensitivity = 0.1f;
    bool menu = false;
    bool firstMouse = true;
    float camLastX = 0.0f;
    float camLastY = 0.0f;
    mutable std::optional<RaycastHit> raycastInfo;

    bool escDown = false;
    bool lDown = false;
    

    // Configurações de movimento


    // Direções calculadas
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    Camera(glm::vec3 startPosition, float startYaw = -90.0f, float startPitch = 0.0f) {
        position = startPosition;
        pitch = 0.0f;
        yaw = -90.0f;
        fov = 75.0f;

        updateVectors();
    }



    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    void processKeyboard(CameraMovement direction, float deltaTime) {
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

    void processMouseMovement( float xOffset, float yOffset, bool constrainPitch = true) {
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

    void processMouseScroll(float yOffset) {
        fov -= yOffset;
        fov = glm::clamp(fov, 1.0f, 75.0f);
        
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }


    glm::vec3 getCamPos() {
        return position;
    }

    void Camera::raycast(float maxDistance, float step,
        const std::function<std::optional<RaycastHit>(glm::ivec3)>& isBlockAir) const
    {
        glm::vec3 dir = glm::normalize(front);
        glm::ivec3 lastBlockPos = glm::floor(position);

        raycastInfo = std::nullopt;  // Limpa antes de começar

        for (float t = 0.0f; t <= maxDistance; t += step) {
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

private:
    void updateVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        // Assume worldUp sempre (0,1,0)
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, front));
    }
};


#endif
