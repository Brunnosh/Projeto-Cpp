#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


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
