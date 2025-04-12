#include <chunk.h>

#include <chrono>
#include <iostream>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Chunk::Chunk(glm::ivec3 pos) {
    worldPos = pos;
    generated = false;
    ready = false;
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

void Chunk::genChunk() {



    auto start = std::chrono::high_resolution_clock::now();

	//teste chunk 15x15x15 solido
    for (int i = 0; i < pow(CHUNKSIZE, 3); i++) {
        chunkData.push_back(Blocks[BlockType::GRASS]);
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

                

				//north
                if (isAirAt(x, y, z - 1, &chunkData)) {

                    vertices.push_back(Vertex(x + 0, y + 0, z + 0, 
                        blockUVs[storedBlock.getType()][FACE::NORTH].uMax,
                        blockUVs[storedBlock.getType()][FACE::NORTH].vMin));

                    vertices.push_back(Vertex(x + 1, y + 0, z + 0, 
                        blockUVs[storedBlock.getType()][FACE::NORTH].uMin,
                        blockUVs[storedBlock.getType()][FACE::NORTH].vMin));

                    vertices.push_back(Vertex(x + 1, y + 1, z + 0,
                        blockUVs[storedBlock.getType()][FACE::NORTH].uMin,
                        blockUVs[storedBlock.getType()][FACE::NORTH].vMax));

                    vertices.push_back(Vertex(x + 0, y + 1, z + 0,
                        blockUVs[storedBlock.getType()][FACE::NORTH].uMax,
                        blockUVs[storedBlock.getType()][FACE::NORTH].vMax));

                    indices.push_back(currentVertex + 1);
                    indices.push_back(currentVertex + 0);
                    indices.push_back(currentVertex + 3);
                    indices.push_back(currentVertex + 3);
                    indices.push_back(currentVertex + 2);
                    indices.push_back(currentVertex + 1);
                    currentVertex += 4;
                }


               

				//south
                if(isAirAt(x,y,z+1,&chunkData)){



                    

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
                }
                

				//east
                if (isAirAt(x + 1, y, z, &chunkData)) {
                    

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

                }
                

				//west

                if (isAirAt(x - 1, y, z, &chunkData)) {

                    

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

                }
                

                //top
                if(isAirAt(x,y+1,z,&chunkData))
                {
                    


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
                }




				//bottom
                if (isAirAt(x, y - 1, z, & chunkData)) {
                    

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

                }
                

			}
		}

	}

    this->generated = true;


    // Fim da medição de tempo
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Estimativa de memória (em bytes)
    size_t chunkDataMem = chunkData.capacity() * sizeof(uint8_t);
    size_t verticesMem = vertices.capacity() * sizeof(Vertex);
    size_t indicesMem = indices.capacity() * sizeof(unsigned int);

    size_t totalMemory = chunkDataMem + verticesMem + indicesMem;

    // Log
    std::cout << "Chunk (X : " << worldPos.x << ", Y: " << worldPos.y << ", Z: " << worldPos.z << ")" << " \n";
    std::cout << "Chunk gerado em " << duration.count() << " segundos\n";
    std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << "\n";
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
    model = glm::translate(model, glm::vec3(worldPos)); //TROCAR ESSE VEC PELA POSICAO NO MUNDO DO CHUNK
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, numberVertexes, GL_UNSIGNED_INT, 0);
}