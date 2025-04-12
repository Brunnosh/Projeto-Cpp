#include <chunk.h>

#include <chrono>
#include <iostream>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void addVertxInfo(FACE face, char x, char y, char z, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int& currentVertex, Block storedBlock);




Chunk::Chunk(glm::ivec3 pos) {
    worldPos = pos;
    generated = false;
    ready = false;
    chunkData = populateChunk(pos);
}

bool Chunk::isAirAt(int x, int y, int z, std::vector<Block>* chunkData) {
    if (x < 0 || x >= CHUNKSIZE ||y < 0 || y >= CHUNKSIZE ||z < 0 || z >= CHUNKSIZE) 
    {
        // Futuramente: consultar chunk vizinho aqui
        return true; // fora do chunk = renderiza face
    }

    int index = x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y; //posicao do chunk "alterada"
    //checar se index é air retornar true
    return (*chunkData)[index].getType() == BlockType::AIR;

}


//NAO MODIFICAR ESSE WORLDDATA EM HIPÓTESE ALGUMA SEM IMPLEMENTAR MUTEX---V
void Chunk::genChunkFaces(std::unordered_map<glm::ivec3, Chunk, Vec3Hash> &WorldData) {



    auto start = std::chrono::high_resolution_clock::now();



    glm::ivec3 thisChunk = this->worldPos;

    glm::ivec3 northChunkPos = glm::ivec3(thisChunk.x, thisChunk.y, thisChunk.z - 1);
    glm::ivec3 southChunkPos = glm::ivec3(thisChunk.x, thisChunk.y, thisChunk.z + 1);
    glm::ivec3 eastChunkPos = glm::ivec3(thisChunk.x + 1, thisChunk.y, thisChunk.z );
    glm::ivec3 westChunkPos = glm::ivec3(thisChunk.x -1, thisChunk.y, thisChunk.z);

    //CHunk de cima e baixo por enquanto sem implementar
    // futuramente quem vai decidir os blocos vai ser a geracao randomizada o mundo, ou seja, se eu fizer agora vai ser tudo vazio, pq se eu criar um chunk hipotetico a mais no Y, nada vai ser renderizado.
    //glm::ivec3 topChunkPos = glm::ivec3(thisChunk.x, thisChunk.y + 1, thisChunk.z);
    //glm::ivec3 botChunkPos = glm::ivec3(thisChunk.x, thisChunk.y -1, thisChunk.z);

    
    std::vector<Block> northChunk;
    std::vector<Block> southChunk;
    std::vector<Block> eastChunk;
    std::vector<Block> westChunk;

    //not check topChunk if y == max height
    std::vector<Block> topChunk;
    std::vector<Block> bottomChunk;

    //da maneira que estou gerando o mundo, basicamente apenas os chunks da direita e de tras do chunk vao j'a ter gerado.

    if (WorldData.find(northChunkPos) != WorldData.end()) {
        
    }
    if (WorldData.find(southChunkPos) != WorldData.end()) {
       
    }
    if (WorldData.find(eastChunkPos) != WorldData.end()) {
        
    }
    if (WorldData.find(westChunkPos) != WorldData.end()) {
        
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

                

			
                if (isAirAt(x, y, z - 1, &chunkData)) {

                    addVertxInfo(FACE::NORTH,x,y,z, vertices, indices, currentVertex, storedBlock);

                }

                if(isAirAt(x,y,z+1,&chunkData)){

                    addVertxInfo(FACE::SOUTH, x, y, z, vertices, indices, currentVertex, storedBlock);

                }

                if (isAirAt(x + 1, y, z, &chunkData)) {
                    addVertxInfo(FACE::EAST, x, y, z, vertices, indices, currentVertex, storedBlock);

                }

                if (isAirAt(x - 1, y, z, &chunkData)) {

                    addVertxInfo(FACE::WEST, x, y, z, vertices, indices, currentVertex, storedBlock);

                }

                if(isAirAt(x,y+1,z,&chunkData))
                {
                    addVertxInfo(FACE::TOP, x, y, z, vertices, indices, currentVertex, storedBlock);

                }
		
                if (isAirAt(x, y - 1, z, & chunkData)) {
                    
                    addVertxInfo(FACE::BOTTOM, x, y, z, vertices, indices, currentVertex, storedBlock);

                }
			}
		}
	}

    this->generated = true;


    // Fim da medição de tempo
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;



    // Log
    
    //std::cout << "Chunk gerado em " << duration.count() << " segundos\n";
    //std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << "\n";
}








void Chunk::render(unsigned int modelLoc) {

    if (!ready) {

        if (generated) {
            

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, X));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uvX));
            glEnableVertexAttribArray(1);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


            ready = true;
        }
        
        return;
    }
   
    size_t numberVertexes = indices.size();
    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(worldPos*15)); //TROCAR ESSE VEC PELA POSICAO NO MUNDO DO CHUNK
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, numberVertexes, GL_UNSIGNED_INT, 0);
}


std::vector<Block> Chunk::populateChunk(glm::ivec3 chunkCoords) {
    //Futuramente usar WorldPos junto com a seed/Noise para gerar os blocos do chunk
    std::vector<Block> tempVec;
    //teste chunk 15x15x15 solido


    for (char x = 0; x < CHUNKSIZE; x++) {
        for (char z = 0; z < CHUNKSIZE; z++) {
            for (char y = 0; y < CHUNKSIZE; y++) {
                if ((chunkCoords.y * 15) < 60) {
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



void addVertxInfo(FACE face, char x, char y, char z, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int& currentVertex, Block storedBlock) {

    const UV uv = blockUVs[storedBlock.getType()][face];
    float uMin = uv.uMin, uMax = uv.uMax, vMin = uv.vMin, vMax = uv.vMax;



    switch (face) {
    case FACE::NORTH:
        vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMin));
        vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMin));
        vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMin, blockUVs[storedBlock.getType()][FACE::NORTH].vMax));
        vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::NORTH].uMax, blockUVs[storedBlock.getType()][FACE::NORTH].vMax));

        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::SOUTH:
        vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin));
        vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMin));
        vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMax, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax));
        vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::SOUTH].uMin, blockUVs[storedBlock.getType()][FACE::SOUTH].vMax));
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::EAST:
        vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMin));
        vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMin));
        vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::EAST].uMin, blockUVs[storedBlock.getType()][FACE::EAST].vMax));
        vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::EAST].uMax, blockUVs[storedBlock.getType()][FACE::EAST].vMax));

        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 1);
        currentVertex += 4;
        break;
    case FACE::WEST:
        vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMin));
        vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMin));
        vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::WEST].uMax, blockUVs[storedBlock.getType()][FACE::WEST].vMax));
        vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::WEST].uMin, blockUVs[storedBlock.getType()][FACE::WEST].vMax));

        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 0);
        currentVertex += 4;
        break;
    case FACE::TOP:
        vertices.push_back(Vertex(x + 0, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMax));
        vertices.push_back(Vertex(x + 1, y + 1, z + 0, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMax));
        vertices.push_back(Vertex(x + 1, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMax, blockUVs[storedBlock.getType()][FACE::TOP].vMin));
        vertices.push_back(Vertex(x + 0, y + 1, z + 1, blockUVs[storedBlock.getType()][FACE::TOP].uMin, blockUVs[storedBlock.getType()][FACE::TOP].vMin));

        indices.push_back(currentVertex + 3);
        indices.push_back(currentVertex + 2);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 1);
        indices.push_back(currentVertex + 0);
        indices.push_back(currentVertex + 3);
        currentVertex += 4;
        break;
    case FACE::BOTTOM:
        vertices.push_back(Vertex(x + 0, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin));
        vertices.push_back(Vertex(x + 1, y + 0, z + 0, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMin));
        vertices.push_back(Vertex(x + 1, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMax, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax));
        vertices.push_back(Vertex(x + 0, y + 0, z + 1, blockUVs[storedBlock.getType()][FACE::BOTTOM].uMin, blockUVs[storedBlock.getType()][FACE::BOTTOM].vMax));

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
