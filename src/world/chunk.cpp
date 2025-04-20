#include <chunk.h>
#include <shader.h>

#include <chrono>
#include <iostream>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



bool Chunk::isAirAt(int x, int y, int z, std::vector<Block>* chunkData, std::vector<Block>* nextChunkData) {
    if (x >= 0 && x < CHUNKSIZE &&
        y >= 0 && y < CHUNKSIZE &&
        z >= 0 && z < CHUNKSIZE) {

        // Dentro do chunk atual
        int index = x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y;
        return (*chunkData)[index].getType() == BlockType::AIR;
    }
    else {
        // Acessar chunk vizinho — ajustar a coordenada
        int nx = (x + CHUNKSIZE) % CHUNKSIZE;
        int ny = (y + CHUNKSIZE) % CHUNKSIZE;
        int nz = (z + CHUNKSIZE) % CHUNKSIZE;

        int index = nx * CHUNKSIZE * CHUNKSIZE + nz * CHUNKSIZE + ny;
        return (*nextChunkData)[index].getType() == BlockType::AIR;
    }

    // Não tem chunk vizinho  considere ar (para forçar renderização da face)
    return true;
}


std::vector<Block> Chunk::populateChunk(glm::ivec3 chunkCoords) {
    //Futuramente usar WorldPos junto com a seed/Noise para gerar os blocos do chunk
    std::vector<Block> tempVec;
    //teste chunk 15x15x15 solido

    //std::cout << "GERANDO CHUNK X: " << chunkCoords.x<< ", Y:  " << chunkCoords.y << ", Z: "<< chunkCoords.z << "\n";

    if (chunkCoords == glm::ivec3(0, 3, 0)) {

        for (int i = 0; i < CHUNKSIZE * CHUNKSIZE * CHUNKSIZE; i++) {
            tempVec.push_back(Blocks[BlockType::AIR]);
        }
        return tempVec;
    }


    for (char x = 0; x < CHUNKSIZE; x++) {
        for (char z = 0; z < CHUNKSIZE; z++) {
            for (char y = 0; y < CHUNKSIZE; y++) {
                if ((chunkCoords.y * CHUNKSIZE) < 60 && (chunkCoords.y * CHUNKSIZE) > 0) {
                    tempVec.push_back(Blocks[BlockType::GRASS]);
                }
                else
                {
                    tempVec.push_back(Blocks[BlockType::AIR]);
                }

            }
        }
    }



    return tempVec;
}

void Chunk::regenMesh(std::unordered_map<glm::ivec3, Chunk, Vec3Hash>& WorldData) {
    //auto start = std::chrono::high_resolution_clock::now();



    this->vertices.clear();
    this->indices.clear();
    this->generated = false;

    genChunkFaces(WorldData);

    this->isEmpty = vertices.empty();
    if (vertices.empty()) {
        return;
    }

    this->generated = true;

    int nextBuffer = (activeBuffer + 1) % 2;

    if (buffers[nextBuffer].VAO == 0)
        glGenVertexArrays(1, &buffers[nextBuffer].VAO);
    if (buffers[nextBuffer].VBO == 0)
        glGenBuffers(1, &buffers[nextBuffer].VBO);
    if (buffers[nextBuffer].EBO == 0)
        glGenBuffers(1, &buffers[nextBuffer].EBO);

    glBindVertexArray(buffers[nextBuffer].VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[nextBuffer].VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, X));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uvX));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normalX));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, lightLevel));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[nextBuffer].EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    activeBuffer = nextBuffer;
    ready = true;

    // Fim da medição de tempo
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = end - start;



    // Log

    //std::cout << "Mesh gerada em: " << duration.count() << " segundos\n";
    //std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << "\n";

}


//NAO MODIFICAR ESSE WORLDDATA EM HIPÓTESE ALGUMA SEM IMPLEMENTAR MUTEX---V
void Chunk::genChunkFaces(std::unordered_map<glm::ivec3, Chunk, Vec3Hash> &WorldData) {

    glm::ivec3 thisChunk = this->worldPos;



    glm::ivec3 northChunkPos = glm::ivec3(thisChunk.x, thisChunk.y, thisChunk.z - 1);
    glm::ivec3 southChunkPos = glm::ivec3(thisChunk.x, thisChunk.y, thisChunk.z + 1);
    glm::ivec3 eastChunkPos = glm::ivec3(thisChunk.x + 1, thisChunk.y, thisChunk.z );
    glm::ivec3 westChunkPos = glm::ivec3(thisChunk.x -1, thisChunk.y, thisChunk.z);

    glm::ivec3 topChunkPos = glm::ivec3(thisChunk.x, thisChunk.y + 1, thisChunk.z);
    glm::ivec3 botChunkPos = glm::ivec3(thisChunk.x, thisChunk.y -1, thisChunk.z);

    
    std::vector<Block> northChunk;
    std::vector<Block> southChunk;
    std::vector<Block> eastChunk;
    std::vector<Block> westChunk; 
    std::vector<Block> topChunk;
    std::vector<Block> bottomChunk;

    //da maneira que estou gerando o mundo, basicamente apenas os chunks da direita e de tras do chunk vao j'a ter gerado.

    if (WorldData.find(northChunkPos) != WorldData.end()) {
        northChunk = WorldData.find(northChunkPos)->second.chunkData;
    }
    else {
        northChunk = populateChunk(northChunkPos);
    }

    if (WorldData.find(southChunkPos) != WorldData.end()) {
        southChunk = WorldData.find(southChunkPos)->second.chunkData;
    }
    else {
        southChunk = populateChunk(southChunkPos);
    }

    if (WorldData.find(eastChunkPos) != WorldData.end()) {
        eastChunk = WorldData.find(eastChunkPos)->second.chunkData;
    }
    else {
        eastChunk = populateChunk(eastChunkPos);
    }

    if (WorldData.find(westChunkPos) != WorldData.end()) {
        westChunk = WorldData.find(westChunkPos)->second.chunkData;
    }
    else {
        westChunk = populateChunk(westChunkPos);
    }

    if (WorldData.find(topChunkPos) != WorldData.end()) {
        topChunk = WorldData.find(topChunkPos)->second.chunkData;
    }
    else {
        topChunk = populateChunk(topChunkPos);
    }

    if (WorldData.find(botChunkPos) != WorldData.end()) {
        bottomChunk = WorldData.find(botChunkPos)->second.chunkData;
    }
    else {
        bottomChunk = populateChunk(botChunkPos);
    }

    unsigned int currentVertex = 0;
	for (char x = 0; x < CHUNKSIZE; x++) {
		for (char z = 0; z < CHUNKSIZE; z++) {
			for (char y = 0; y < CHUNKSIZE; y++) {

                //escolher bloco no chunkdata[i]
                int index = x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y;
                Block storedBlock = chunkData[index];
                
                
                if (storedBlock.getType() == BlockType::AIR)
                    continue;

                

			
                if (isAirAt(x, y, z - 1, &chunkData, &northChunk)) {

                    addVertxInfo(FACE::NORTH,x,y,z, vertices, indices, currentVertex, storedBlock);

                }

                if(isAirAt(x,y,z+1,&chunkData, &southChunk)){

                    addVertxInfo(FACE::SOUTH, x, y, z, vertices, indices, currentVertex, storedBlock);

                }

                if (isAirAt(x + 1, y, z, &chunkData, &eastChunk)) {
                    addVertxInfo(FACE::EAST, x, y, z, vertices, indices, currentVertex, storedBlock);

                }

                if (isAirAt(x - 1, y, z, &chunkData, &westChunk)) {

                    addVertxInfo(FACE::WEST, x, y, z, vertices, indices, currentVertex, storedBlock);

                }

                if(isAirAt(x,y+1,z,&chunkData, &topChunk))
                {
                    addVertxInfo(FACE::TOP, x, y, z, vertices, indices, currentVertex, storedBlock);

                }
		
                if (isAirAt(x, y - 1, z, & chunkData, &bottomChunk)) {
                    
                    addVertxInfo(FACE::BOTTOM, x, y, z, vertices, indices, currentVertex, storedBlock);

                }
			}
		}
	}



}










void Chunk::addVertxInfo(FACE face, char x, char y, char z, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int& currentVertex, Block storedBlock) {

    const UV uv = blockUVs[storedBlock.getType()][face];
    float uMin = uv.uMin, uMax = uv.uMax, vMin = uv.vMin, vMax = uv.vMax;



    switch (face) {
    case FACE::NORTH:
        vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMin, 0, 0, -1, std::max(storedBlock.getSkyLight(),storedBlock.getBlockLight()) ));
        vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMin, 0, 0, -1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMax, 0, 0, -1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMax, 0, 0, -1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::SOUTH:
        vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax, 0, 0, 1, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::EAST:
        vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMin, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMin, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMax, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMax, 1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::WEST:
        vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMin, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMin, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMax, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMax, -1, 0, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::TOP:
        vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMax, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMax, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMin, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMin, 0, 1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 3);
        currentVertex += 4;
        break;
    case FACE::BOTTOM:
        vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));
        vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax, 0, -1, 0, std::max(storedBlock.getSkyLight(), storedBlock.getBlockLight())));

        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    }
}



void Chunk::render(unsigned int modelLoc) {


    if (!ready || buffers[activeBuffer].VAO == 0) return;

    glBindVertexArray(buffers[activeBuffer].VAO);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos * CHUNKSIZE));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}


