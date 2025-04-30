#pragma once

#include <camera.h>
#include <world.h>
#include <voxelVertex.h>

#include <unordered_map>
#include <queue>
#include <vector>
#include <set>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <mutex>

struct chunkBuffers {
    GLuint VAO = 0, VBO = 0, EBO = 0;
};

struct ChunkRenderData {
    chunkBuffers buffers;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //watervertices
    //waterindices
    bool uploaded = false;
    bool isEmpty = false;
};

class Renderer {
private:
    std::unordered_map<glm::ivec3, ChunkRenderData, Vec3Hash> chunkRenderMap;
    std::queue<glm::ivec3> dirtyChunks;
    std::unordered_set<glm::ivec3, Vec3Hash> dirtyChunksControl;
    
    std::mutex markedChunkMutex;
public:
  
    World* worldReference;

public:

    void markChunkRemesh(const glm::ivec3& pos);
    void markChunkRemesh(std::queue<glm::ivec3> & tempQueue);
    void remeshMarkedChunks();
    void renderChunks(unsigned int modelLoc, int& drawcallCount);
    void cleanup();
    void genFaces(const glm::ivec3& pos, chunkObject& chunkObject);
    void uploadToGPU(const glm::ivec3& pos);

private:
    void Renderer::setVertex(int x, int y, int z, Block& storedBlock, FACE face, ChunkRenderData& renderData, unsigned int& currentVertex);
    void generateMesh(Chunk& chunk, ChunkRenderData& renderData);
    void uploadToGPU(ChunkRenderData& renderData);
    bool shouldRenderFace(const glm::ivec3& chunkPos, int x, int y, int z, FACE face);
    bool Renderer::canGenerateFaces(const glm::ivec3& chunkPos);
};