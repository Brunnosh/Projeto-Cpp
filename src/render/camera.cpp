#include <camera.h>
#include <world.h>
#include <shader.h>
#include <window.h>

float outlineVertices[] = {
    // frente
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // trás
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    // esquerda
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // direita
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,

     // topo
     -0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
     -0.5f,  0.5f, -0.5f,
     -0.5f,  0.5f,  0.5f,

     // fundo
     -0.5f, -0.5f,  0.5f,
     -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f,  0.5f,
     -0.5f, -0.5f,  0.5f,
};


void Camera::selectBlock() {
    selectedBlock = raycastInfo.value().copiedBlock;
}

void Camera::selectNextBlock() {
    if (selectedBlock == nullptr) return;

    // Pega o tipo atual
    int type = static_cast<int>(selectedBlock->getType());

    // Calcula o próximo tipo
    int nextType = (type + 1) % static_cast<int>(BlockType::AMOUNT);

    // Converte de volta pra enum
    BlockType nextBlockType = static_cast<BlockType>(nextType);

    // Acessa o bloco correspondente
    auto it = Blocks.find(nextBlockType);
    if (it != Blocks.end()) {
        selectedBlock = const_cast<Block*>(&it->second);
    }
}

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

void Camera::update(World& world, Window & window, int& drawCallCount) {
    //Calc raycast
    
    
    
    raycast([&](glm::ivec3 pos) {
        return world.returnRayCastHit(pos);
        });
    


    //Draw block outline
    
    drawBlockOutline(window, drawCallCount);
    
    //Collision detection




    //Physics calculation



}

GLuint outlineVAO, outlineVBO;
void Camera::drawBlockOutline(Window& window, int& drawCallCount) {
    if (raycastInfo.has_value()) {

        



        glGenVertexArrays(1, &outlineVAO);
        glGenBuffers(1, &outlineVBO);

        glBindVertexArray(outlineVAO);

        glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(outlineVertices), outlineVertices, GL_STATIC_DRAW);

        // posição (somente vec3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // unbind (opcional)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f); // evita z-fighting com os blocos

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // modo wireframe

        Shaders[shaderType::OUTLINE].use();
        Shaders[shaderType::OUTLINE].setMat4("projection", glm::perspective(glm::radians(fov), (float)window.WIDHT / (float)window.HEIGHT, 0.01f, 5000.0f));
        Shaders[shaderType::OUTLINE].setMat4("view", GetViewMatrix());

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(raycastInfo.value().blockWorldPos.x + 0.5f, raycastInfo.value().blockWorldPos.y + 0.5f, raycastInfo.value().blockWorldPos.z + 0.5f));
        Shaders[shaderType::OUTLINE].setMat4("model", model);

        Shaders[shaderType::OUTLINE].setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f)); // amarelo

        glBindVertexArray(outlineVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); // mesmo cubo do bloco normal
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // volta ao normal
        glDisable(GL_POLYGON_OFFSET_LINE);
        glDeleteVertexArrays(1, &outlineVAO);
        glDeleteBuffers(1, &outlineVBO);
        drawCallCount++;
        Shaders[shaderType::MAIN].use();

    }
}