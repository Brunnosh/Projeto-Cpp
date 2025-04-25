#include "Renderer.h"
#include <glad/glad.h>
#include <shader.h>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void Renderer::renderChunks(unsigned int modelLoc) {
    for (auto& [pos, data] : chunkRenderMap) {
        if (!data.uploaded) continue;

        //differentiate water rendering here
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos * CHUNKSIZE));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

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

    glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, X));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uvX));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normalX));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, lightLevel));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderData.indices.size() * sizeof(unsigned int), renderData.indices.data(), GL_STATIC_DRAW);

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

    unsigned int currentVertex = 0;
    for (int x = 0; x < CHUNKSIZE ; ++x)
        for (int y = 0; y < CHUNKSIZE ; ++y)
            for (int z = 0; z < CHUNKSIZE ; ++z) {
                Block& storedBlock = chunk.getBlock(x, y, z);
                if (storedBlock.getType() == BlockType::AIR) continue;

                //future checks for water blocks, transparent blocks (ADD TO renderData as "WaterVertices" or "Transparentvertices idk"



                for (int f = 0; f < 6; ++f) {
                    if (isFaceVisible(chunk.worldPos, x, y, z, FACE(f))) {
                        setVertex(x,y,z, storedBlock, FACE(f), renderData, currentVertex);
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

void Renderer::setVertex(int x, int y, int z, Block & storedBlock, FACE face, ChunkRenderData& renderData, unsigned int & currentVertex) {
    
    const UV uv = blockUVs[storedBlock.getType()][face];
    float uMin = uv.uMin, uMax = uv.uMax, vMin = uv.vMin, vMax = uv.vMax;

    

    switch (face) {
    case FACE::NORTH:
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMin, 0, 0, -1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMin, 0, 0, -1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMax, 0, 0, -1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMax, 0, 0, -1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::SOUTH:
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::EAST:
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMin, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMin, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMax, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMax, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::WEST:
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMin, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMin, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMax, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMax, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::TOP:
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMax, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMax, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMin, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMin, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 3);
        currentVertex += 4;
        break;
    case FACE::BOTTOM:
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    }
}