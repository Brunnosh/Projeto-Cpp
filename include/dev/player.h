#ifndef PLAYER_H
#define PLAYER_H

#include <camera.h>

class Player {
public:
    Camera camera;
    glm::vec3 playerPos;

    // Estado do jogador
    bool menu = false;
    bool firstMouse = true;
    float camLastX = 0.0f;
    float camLastY = 0.0f;

public:
    Player();
    Player(glm::vec3 position);

    short getRenderDistance() const {
        return camera.renderDist;
    }

    glm::vec3 getPosition() const;

    void movePlayer();
    void rotateCamera();

};

#endif // PLAYER_H
