// Renderer.h
#pragma once

#include <unordered_map>
#include <queue>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <camera.h>
#include <world.h>
#include <voxelVertex.h>

struct chunkBuffers {
    GLuint VAO = 0, VBO = 0, EBO = 0;
};

struct ChunkRenderData {
    chunkBuffers buffers;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

class Renderer {
private:
    std::unordered_map<glm::ivec3, ChunkRenderData, Vec3Hash> chunkRenderMap;
    std::queue<glm::ivec3> dirtyChunks;

public:

    void rebuildDirtyChunks();
    void renderChunks(const Camera& camera);
    void cleanup();

private:
    void generateMesh(const Chunk& chunk, ChunkRenderData& renderData);
    void uploadToGPU(ChunkRenderData& renderData);
    bool isFaceVisible(const glm::ivec3& chunkPos, int x, int y, int z, FACE face);
};