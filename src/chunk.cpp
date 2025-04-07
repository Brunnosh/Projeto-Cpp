#include <chunk.h>

#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




void Chunk::genChunk() {

	//teste chunk 15x15x15 só de dirt, sem meshing nenhum
    for (int i = 0; i < pow(CHUNKSIZE, 3); i++) {
        chunkData.push_back(1);
    }

    

    unsigned int currentVertex = 0;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {


                //escolher bloco no chunkdata[i]
                int index = x * CHUNKSIZE * CHUNKSIZE + z * CHUNKSIZE + y;
                if (chunkData[index] == 0)
                    continue;

                Block storedBlock = Blocks[chunkData[index]];

				//north

				vertices.push_back(Vertex(x + 0, y + 0, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 0, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 1, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 0, y + 1, z + 0, 0, 0));

                indices.push_back(currentVertex + 1);
                indices.push_back(currentVertex + 0);
                indices.push_back(currentVertex + 3);
                indices.push_back(currentVertex + 3);
                indices.push_back(currentVertex + 2);
                indices.push_back(currentVertex + 1);
                currentVertex += 4;
               

				//south


                vertices.push_back(Vertex(x + 0, y + 0, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 0, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 1, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 0, y + 1, z + 1, 0, 0));
                indices.push_back(currentVertex + 0);
                indices.push_back(currentVertex + 1);
                indices.push_back(currentVertex + 2);
                indices.push_back(currentVertex + 2);
                indices.push_back(currentVertex + 3);
                indices.push_back(currentVertex + 0);
                currentVertex += 4;

				//east

                vertices.push_back(Vertex(x + 1, y + 0, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 0, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 1, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 1, z + 0, 0, 0));
 
                indices.push_back(currentVertex + 1);
                indices.push_back(currentVertex + 0);
                indices.push_back(currentVertex + 3);
                indices.push_back(currentVertex + 3);
                indices.push_back(currentVertex + 2);
                indices.push_back(currentVertex + 1);
                currentVertex += 4;


				//west


                vertices.push_back(Vertex(x + 0, y + 0, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 0, y + 0, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 0, y + 1, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 0, y + 1, z + 0, 0, 0));

                indices.push_back(currentVertex + 0);
                indices.push_back(currentVertex + 1);
                indices.push_back(currentVertex + 2);
                indices.push_back(currentVertex + 2);
                indices.push_back(currentVertex + 3);
                indices.push_back(currentVertex + 0);
                currentVertex += 4;


                //top

                vertices.push_back(Vertex(x + 0, y + 1, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 1, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 1, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 0, y + 1, z + 1, 0, 0));

                indices.push_back(currentVertex + 3);
                indices.push_back(currentVertex + 2);
                indices.push_back(currentVertex + 1);
                indices.push_back(currentVertex + 1);
                indices.push_back(currentVertex + 0);
                indices.push_back(currentVertex + 3);
                currentVertex += 4;


				//bottom


                vertices.push_back(Vertex(x + 0, y + 0, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 0, z + 0, 0, 0));
                vertices.push_back(Vertex(x + 1, y + 0, z + 1, 0, 0));
                vertices.push_back(Vertex(x + 0, y + 0, z + 1, 0, 0));

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

    this->generated = true;
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
            glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uvX));
            glEnableVertexAttribArray(1);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


            ready = true;
        }
        return;
    }

    int numberVertexes = indices.size();
    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, 0)); //TROCAR ESSE VEC PELA POSICAO NO MUNDO DO CHUNK
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, numberVertexes, GL_UNSIGNED_INT, 0);
}