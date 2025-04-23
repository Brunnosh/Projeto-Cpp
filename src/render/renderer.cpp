#include "Renderer.h"
#include <glad/glad.h>






void Renderer::rebuildDirtyChunks() {
    while (!dirtyChunks.empty()) {
        glm::ivec3 pos = dirtyChunks.front();
        dirtyChunks.pop();

        auto obj = world->getChunk(pos);
        if (!obj || obj->isEmpty) continue;

        auto& data = chunkRenderMap[pos];
        generateMesh(*obj, data);
        uploadToGPU(data);
    }
}

void Renderer::renderChunks(const Camera& camera) {
    for (auto& [pos, data] : chunkRenderMap) {
        if (!data.uploaded) continue;

        glBindVertexArray(data.buffers.VAO);
        glDrawElements(GL_TRIANGLES, data.indices.size(), GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void Renderer::cleanup() {
    for (auto& [_, data] : chunkRenderMap) {
        glDeleteBuffers(1, &data.buffers.VBO);
        glDeleteBuffers(1, &data.buffers.EBO);
        glDeleteVertexArrays(1, &data.buffers.VAO);
    }
    chunkRenderMap.clear();
}

void Renderer::generateMesh(const Chunk& chunk, ChunkRenderData& renderData) {
    renderData.vertices.clear();
    renderData.indices.clear();
    unsigned int indexOffset = 0;

    for (int x = 0; x < CHUNKSIZE; ++x)
        for (int y = 0; y < CHUNKSIZE; ++y)
            for (int z = 0; z < CHUNKSIZE; ++z) {
                const Block& block = chunk.getBlock(x, y, z);
                if (block.getType() == BlockType::AIR) continue;

                for (int f = 0; f < 6; ++f) {
                    if (isFaceVisible(chunk.worldPos, x, y, z, FACE(f))) {
                        // Adicionar aqui os vertices e indices do bloco para a face `f`.
                        // renderData.vertices.push_back(...);
                        // renderData.indices.push_back(indexOffset + ...);
                        // indexOffset += 4;
                    }
                }
            }
}

void Renderer::uploadToGPU(ChunkRenderData& renderData) {
    if (renderData.buffers.VAO == 0) glGenVertexArrays(1, &renderData.buffers.VAO);
    if (renderData.buffers.VBO == 0) glGenBuffers(1, &renderData.buffers.VBO);
    if (renderData.buffers.EBO == 0) glGenBuffers(1, &renderData.buffers.EBO);

    glBindVertexArray(renderData.buffers.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderData.buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER, renderData.vertices.size() * sizeof(Vertex), renderData.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderData.indices.size() * sizeof(unsigned int), renderData.indices.data(), GL_STATIC_DRAW);

    // Atributos de vértice: posição, normal, UV etc
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Adicione outros atributos conforme seu Vertex

    glBindVertexArray(0);
    renderData.uploaded = true;
}

bool Renderer::isFaceVisible(const glm::ivec3& chunkPos, int x, int y, int z, FACE face) {
    glm::ivec3 offset = { 0, 0, 0 };
    switch (face) {
    case FACE::NORTH: offset.z = -1; break;
    case FACE::SOUTH: offset.z = 1; break;
    case FACE::EAST: offset.x = 1; break;
    case FACE::WEST: offset.x = -1; break;
    case FACE::TOP: offset.y = 1; break;
    case FACE::BOTTOM: offset.y = -1; break;
    }
    glm::ivec3 neighborPos = glm::ivec3(x, y, z) + offset;

    const Block& neighborBlock = world->getBlockGlobal(chunkPos * CHUNKSIZE + neighborPos);
    return neighborBlock.getType() == BlockType::AIR;
}
