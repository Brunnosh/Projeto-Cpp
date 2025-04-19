#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <optional>
#include <functional>
#include <chunk.h>

class Window;
class World;

struct RaycastHit {
    Chunk* chunk;
    glm::ivec3 blockRelativePos;
    glm::ivec3 blockWorldPos;
    Block* copiedBlock;
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
    glm::vec3 position;
    float yaw;
    float pitch;
    float fov;


    bool escDown = false;
    bool lDown = false;
    short renderDist = 8;
    float moveSpeed = 10.0f;
    float mouseSensitivity = 0.1f;
    bool menu = false;
    bool firstMouse = true;
    float camLastX = 0.0f;
    float camLastY = 0.0f;


    mutable std::optional<RaycastHit> raycastInfo;
    float cameraReach = 6.0f;
    float raycastStep = 0.01f;
    Block * selectedBlock = nullptr;


    

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

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::vec3 getCamPos() {
        return position;
    }

    void processKeyboard(CameraMovement direction, float deltaTime);

    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

    void processMouseScroll(float yOffset);

    void Camera::raycast(const std::function<std::optional<RaycastHit>(glm::ivec3)>& isBlockAir) const;

    void selectBlock();

    void selectNextBlock();

    void update(World& world, Window & window);

    void drawBlockOutline(Window& window);
    

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
