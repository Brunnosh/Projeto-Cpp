#include "Renderer.h"
#include <glad/glad.h>
#include <shader.h>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Renderer::markChunkRemesh(const glm::ivec3& pos) {

    //decide via chunkState which kind of update it is.
    std::lock_guard<std::mutex> lock(markedChunkMutex);

    if (dirtyChunksControl.find(pos) == dirtyChunksControl.end()) {
        dirtyChunks.push(pos);
        dirtyChunksControl.insert(pos);
    }
    
}

void Renderer::markChunkRemesh(std::queue<glm::ivec3>& tempQueue) {
    

    while (!tempQueue.empty()) {
        glm::ivec3 pos = tempQueue.front();
        tempQueue.pop();
        markChunkRemesh(pos);
    }

    
}
//arrumar isso pra genreciar todas as updates de mesh
void Renderer::remeshMarkedChunks() {
    

    
    std::lock_guard<std::mutex> lock(markedChunkMutex);
    
    while (!dirtyChunks.empty()) {
        glm::ivec3 pos = dirtyChunks.front();
        dirtyChunks.pop();
        dirtyChunksControl.erase(pos);

        

        auto& worldData = worldReference->getWorldDataRef();
        auto it = worldData.find(pos);
        if (it == worldData.end() || !it->second.chunk ) continue;

        

        if (it->second.state != chunkState::DIRTY || !canGenerateFaces(pos)) {
            
            dirtyChunks.push(pos);
            dirtyChunksControl.insert(pos);
            
            return;
        }
        
        genFaces(pos, it->second);        
        uploadToGPU(pos);
    }
    
}


void Renderer::renderChunks(unsigned int modelLoc, int& drawcallCount) {
    for (auto& [pos, data] : chunkRenderMap) {
        if (!data.uploaded || data.isEmpty ) { continue; }
        

        //differentiate water rendering here
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos * CHUNKSIZE));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(data.buffers.VAO);
        glDrawElements(GL_TRIANGLES, data.indices.size(), GL_UNSIGNED_INT, 0);
        drawcallCount++;
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

    if (renderData.buffers.VAO != 0) {
        glDeleteVertexArrays(1, &renderData.buffers.VAO);
        renderData.buffers.VAO = 0;
    }
    if (renderData.buffers.VBO != 0) {
        glDeleteBuffers(1, &renderData.buffers.VBO);
        renderData.buffers.VBO = 0;
    }
    if (renderData.buffers.EBO != 0) {
        glDeleteBuffers(1, &renderData.buffers.EBO);
        renderData.buffers.EBO = 0;
    }

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

    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, lightByte));
    glEnableVertexAttribArray(3);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData.buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderData.indices.size() * sizeof(unsigned int), renderData.indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    renderData.uploaded = true;
}

void Renderer::genFaces(const glm::ivec3& pos, chunkObject& chunkObject) {



    auto it = chunkRenderMap.find(pos);
    if (it != chunkRenderMap.end()) {
        it->second.vertices.clear();
        it->second.indices.clear();
        it->second.uploaded = false; // Resetar se quiser
    }

    ChunkRenderData renderData;

    if (chunkObject.chunk->isEmpty) {
        renderData.isEmpty = true;
        renderData.vertices.clear();
        renderData.indices.clear();
        chunkRenderMap[pos] = renderData;
        uploadToGPU(pos);
        chunkObject.state = chunkState::READY;
        return;
    }


    generateMesh(*chunkObject.chunk, renderData);
    chunkRenderMap[pos] = renderData;
    uploadToGPU(pos);
    chunkObject.state = chunkState::READY;
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
                    if (shouldRenderFace(chunk.worldPos, x, y, z, FACE(f))) {
                        setVertex(x,y,z, storedBlock, FACE(f), renderData, currentVertex);
                    }
                }
            }

}



bool Renderer::shouldRenderFace(const glm::ivec3& chunkPos, int x, int y, int z, FACE face) {
  
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


    //INSIDE CHUNK
    if (neighborPos.x >= 0 && neighborPos.x < CHUNKSIZE &&
        neighborPos.y >= 0 && neighborPos.y < CHUNKSIZE &&
        neighborPos.z >= 0 && neighborPos.z < CHUNKSIZE) 
    {

        return worldReference->isBlockAir(chunkPos, neighborPos.x, neighborPos.y, neighborPos.z);

    }
    else {
        glm::ivec3 neighborChunkPos = chunkPos;
        if (neighborPos.x < 0) neighborChunkPos.x -= 1;
        else if (neighborPos.x >= CHUNKSIZE) neighborChunkPos.x += 1;

        if (neighborPos.y < 0) neighborChunkPos.y -= 1;
        else if (neighborPos.y >= CHUNKSIZE) neighborChunkPos.y += 1;

        if (neighborPos.z < 0) neighborChunkPos.z -= 1;
        else if (neighborPos.z >= CHUNKSIZE) neighborChunkPos.z += 1;

        // Agora, qual posição local dentro do chunk vizinho?
        int nx = (neighborPos.x + CHUNKSIZE) % CHUNKSIZE;
        int ny = (neighborPos.y + CHUNKSIZE) % CHUNKSIZE;
        int nz = (neighborPos.z + CHUNKSIZE) % CHUNKSIZE;

       

        if ((int)worldReference->getChunkState(neighborChunkPos) < 2) {
            return true;
        }

        // Se o chunk vizinho existe, checa o bloco dele
        return worldReference->isBlockAir(neighborChunkPos, nx, ny, nz);

    }


    return true;
}

void Renderer::setVertex(int x, int y, int z, Block & storedBlock, FACE face, ChunkRenderData& renderData, unsigned int & currentVertex) {
    
    const UV uv = blockUVs[storedBlock.getType()][face];
    float uMin = uv.uMin, uMax = uv.uMax, vMin = uv.vMin, vMax = uv.vMax;

    
    //CALCULATE LIGHTING PER VERTEX BASED ON NEIGHBORING BLOCKS
    /*
    
    Face Norte (Z-)  Pega luz dos blocos para o norte (z-1).

    Face Sul (Z+)  Pega luz dos blocos para o sul (z+1).

    Face Leste (X+)  Pega luz dos blocos para o leste (x+1).

    Face Oeste (X-)  Pega luz dos blocos para o oeste (x-1).

    Face Topo (Y+)  Pega luz dos blocos acima (y+1).

    Face Fundo (Y-)  Pega luz dos blocos abaixo (y-1).

    */
    switch (face) {
    case FACE::NORTH:
        
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMin, 0, 0, -1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMin, 0, 0, -1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMax, 0, 0, -1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMax, 0, 0, -1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));

        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::SOUTH:
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin, 0, 0, 1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin, 0, 0, 1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax, 0, 0, 1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax, 0, 0, 1, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::EAST:
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMin, 1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMin, 1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMax, 1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMax, 1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));

        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::WEST:
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMin, -1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMin, -1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMax, -1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMax, -1, 0, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));

        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::TOP:
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMax, 0, 1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMax, 0, 1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMin, 0, 1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMin, 0, 1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));

        renderData.indices.push_back(currentVertex + 3);
        renderData.indices.push_back(currentVertex + 2);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 1);
        renderData.indices.push_back(currentVertex + 0);
        renderData.indices.push_back(currentVertex + 3);
        currentVertex += 4;
        break;
    case FACE::BOTTOM:
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin, 0, -1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin, 0, -1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax, 0, -1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));
        renderData.vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax, 0, -1, 0, storedBlock.getSkyLight(), storedBlock.getBlockLight()));

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

bool Renderer::canGenerateFaces(const glm::ivec3& chunkPos) {
    static const glm::ivec3 neighborOffsets[6] = {
        { 1, 0, 0 }, {-1, 0, 0 },
        { 0, 1, 0 }, { 0,-1, 0 },
        { 0, 0, 1 }, { 0, 0,-1 }
    };

    for (const auto& offset : neighborOffsets) {
        glm::ivec3 neighborChunk = chunkPos + offset;

        if ((int)worldReference->getChunkState(neighborChunk) < 2 && (int)worldReference->getChunkState(neighborChunk) != (int)chunkState::OUTSIDE_RENDER_DISTANCE) {
            // Se qualquer vizinho ainda não existir ou não estiver gerado
            return false;
        }
    }

    return true; // Todos vizinhos prontos
}
