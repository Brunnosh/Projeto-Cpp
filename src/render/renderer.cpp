#include "Renderer.h"
#include <glad/glad.h>
#include <shader.h>

void Renderer::rebuildDirtyChunks(const std::unordered_map<glm::ivec3, chunkObject, Vec3Hash>& worldData) {
    while (!dirtyChunks.empty()) {
        glm::ivec3 pos = dirtyChunks.front();
        dirtyChunks.pop();

        auto it = worldData.find(pos);
        if (it == worldData.end() || !it->second.chunk || it->second.chunk->isEmpty) continue;

        genFaces(pos, *it->second.chunk);
        uploadToGPU(pos);
    }
}

void Renderer::renderChunks() {
    for (auto& [pos, data] : chunkRenderMap) {
        if (!data.uploaded) continue;

        //differentiate water rendering here


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

void Renderer::uploadToGPU(const glm::ivec3& pos) {
    auto it = chunkRenderMap.find(pos);
    if (it == chunkRenderMap.end()) return;

    ChunkRenderData& renderData = it->second;
    uploadToGPU(renderData);
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    renderData.uploaded = true;
}

void Renderer::genFaces(const glm::ivec3& pos, Chunk& chunk) {
    ChunkRenderData renderData;
    generateMesh(chunk, renderData);
    chunkRenderMap[pos] = renderData;
}


void Renderer::generateMesh(Chunk& chunk, ChunkRenderData& renderData) {
    renderData.vertices.clear();
    renderData.indices.clear();

    unsigned int indexOffset = 0;
    for (int x = 0; x < CHUNKSIZE -1; ++x)
        for (int y = 0; y < CHUNKSIZE - 1; ++y)
            for (int z = 0; z < CHUNKSIZE - 1; ++z) {
                const Block& block = chunk.getBlock(x, y, z);
                if (block.getType() == BlockType::AIR) continue;

                //future checks for water blocks, transparent blocks (ADD TO renderData as "WaterVertices" or "Transparentvertices idk"



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



bool Renderer::isFaceVisible(const glm::ivec3& chunkPos, int x, int y, int z, FACE face) {
    glm::ivec3 offset = { 0, 0, 0 };
    switch (face) {
    case FACE::NORTH: offset.z = -1; break;
    case FACE::SOUTH: offset.z = 1; break;
    case FACE::EAST:  offset.x = 1; break;
    case FACE::WEST:  offset.x = -1; break;
    case FACE::TOP:   offset.y = 1; break;
    case FACE::BOTTOM:offset.y = -1; break;
    }
    glm::ivec3 neighborPos = glm::ivec3(x, y, z) + offset;

    //INCOMPLETE FUNCTION, NEEDS TO ACCOUNT FOR CHUNK BOUNDARIES

    return true;
}