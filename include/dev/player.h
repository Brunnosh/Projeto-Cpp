#ifndef PLAYER_H
#define PLAYER_H

#include <camera.h>

class Player {
public:
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 playerPos;

    // Estado do jogador
    bool menu = false;
    bool firstMouse = true;
    float camLastX = 0.0f;
    float camLastY = 0.0f;
    float moveSpeed = 10.0f;
    float mouseSensitivity = 0.1f;

public:
    Player(glm::vec3 position);

    short getRenderDistance() const {
        return camera.renderDist;
    }

    glm::vec3 getPosition() const;

    void calcPlayerMovement(CameraMovement direction, float deltaTime);
    void movePlayer(CameraMovement direction, float deltaTime);
    void rotateCamera(float xOffset, float yOffset, bool constrainPitch);

};

#endif // PLAYER_H
