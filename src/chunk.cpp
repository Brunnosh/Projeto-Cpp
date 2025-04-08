#include <chunk.h>

#include <chrono>
#include <iostream>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

	//teste chunk 15x15x15 só de dirt, sem meshing nenhum
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




                uint8_t northFace = blockTextures[storedBlock.getType()][FACE::NORTH];
                UV northFaceUV = storedBlock.computeUV(northFace);
                

				//north
                if (isAirAt(x, y, z - 1, &chunkData)) {
                    vertices.push_back(Vertex(x + 0, y + 0, z + 0, northFaceUV.uMax, northFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 0, z + 0, northFaceUV.uMin, northFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 1, z + 0, northFaceUV.uMin, northFaceUV.vMax));
                    vertices.push_back(Vertex(x + 0, y + 1, z + 0, northFaceUV.uMax, northFaceUV.vMax));

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

                    uint8_t southFace = blockTextures[storedBlock.getType()][FACE::SOUTH];
                    UV southFaceUV = storedBlock.computeUV(southFace);



                    vertices.push_back(Vertex(x + 0, y + 0, z + 1, southFaceUV.uMin, southFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 0, z + 1, southFaceUV.uMax, southFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 1, z + 1, southFaceUV.uMax, southFaceUV.vMax));
                    vertices.push_back(Vertex(x + 0, y + 1, z + 1, southFaceUV.uMin, southFaceUV.vMax));
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
                    uint8_t eastFace = blockTextures[storedBlock.getType()][FACE::EAST];
                    UV eastFaceUV = storedBlock.computeUV(eastFace);

                    vertices.push_back(Vertex(x + 1, y + 0, z + 0, eastFaceUV.uMax, eastFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 0, z + 1, eastFaceUV.uMin, eastFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 1, z + 1, eastFaceUV.uMin, eastFaceUV.vMax));
                    vertices.push_back(Vertex(x + 1, y + 1, z + 0, eastFaceUV.uMax, eastFaceUV.vMax));

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
                    uint8_t westFace = blockTextures[storedBlock.getType()][FACE::WEST];
                    UV westFaceUV = storedBlock.computeUV(westFace);


                    vertices.push_back(Vertex(x + 0, y + 0, z + 0, westFaceUV.uMin, westFaceUV.vMin));
                    vertices.push_back(Vertex(x + 0, y + 0, z + 1, westFaceUV.uMax, westFaceUV.vMin));
                    vertices.push_back(Vertex(x + 0, y + 1, z + 1, westFaceUV.uMax, westFaceUV.vMax));
                    vertices.push_back(Vertex(x + 0, y + 1, z + 0, westFaceUV.uMin, westFaceUV.vMax));

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
                    uint8_t topFace = blockTextures[storedBlock.getType()][FACE::TOP];
                    UV topFaceUV = storedBlock.computeUV(topFace);



                    vertices.push_back(Vertex(x + 0, y + 1, z + 0, topFaceUV.uMin, topFaceUV.vMax));
                    vertices.push_back(Vertex(x + 1, y + 1, z + 0, topFaceUV.uMax, topFaceUV.vMax));
                    vertices.push_back(Vertex(x + 1, y + 1, z + 1, topFaceUV.uMax, topFaceUV.vMin));
                    vertices.push_back(Vertex(x + 0, y + 1, z + 1, topFaceUV.uMin, topFaceUV.vMin));

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
                    uint8_t bottomFace = blockTextures[storedBlock.getType()][FACE::BOTTOM];
                    UV bottomFaceUV = storedBlock.computeUV(bottomFace);


                    vertices.push_back(Vertex(x + 0, y + 0, z + 0, bottomFaceUV.uMin, bottomFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 0, z + 0, bottomFaceUV.uMax, bottomFaceUV.vMin));
                    vertices.push_back(Vertex(x + 1, y + 0, z + 1, bottomFaceUV.uMax, bottomFaceUV.vMax));
                    vertices.push_back(Vertex(x + 0, y + 0, z + 1, bottomFaceUV.uMin, bottomFaceUV.vMax));

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
    std::cout << "Chunk gerado em " << duration.count() << " segundos\n";
    std::cout << "Memoria usada:\n";
    std::cout << "  chunkData: " << chunkDataMem / 1024.0 << " KB\n";
    std::cout << "  vertices : " << verticesMem / 1024.0 << " KB\n";
    std::cout << "  indices  : " << indicesMem / 1024.0 << " KB\n";
    std::cout << "  Total    : " << totalMemory / 1024.0 << " KB\n";
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
    model = glm::translate(model, glm::vec3(0, 0, 0)); //TROCAR ESSE VEC PELA POSICAO NO MUNDO DO CHUNK
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, numberVertexes, GL_UNSIGNED_INT, 0);
}