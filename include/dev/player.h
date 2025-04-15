#ifndef PLAYER_H
#define PLAYER_H

#include <camera.h>

class Player {
public:
    Camera camera;

    // Estado do jogador
    bool menu = false;
    bool firstMouse = true;
    float camLastX = 0.0f;
    float camLastY = 0.0f;

public:
    Player();

    short getRenderDistance() const {
        return camera.renderDist;
    }

    glm::vec3 getPosition() const;
};

#endif // PLAYER_H
