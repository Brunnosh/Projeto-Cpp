#include <world.h>


float saveTimer = 0.0f;
float saveFrequency = 5.0f;

World::World(Camera& camera){
	this->lastPlayerPos = glm::vec3(0.0f, 0.0f, 0.0f); // no futuro ler do arquvi do mundo


	camera.position = lastPlayerPos;
}


void World::update(Camera& camera,float deltaTime, unsigned int modelLoc) {
	//Salva apenas a posicao do player, no futuro, salvar tudo do world para arquivo (chunks, camera, entidades, etc)
	saveTimer += deltaTime; 

	if (saveTimer >= saveFrequency) {
		lastPlayerPos = camera.getCamPos(); 
		saveTimer = 0.0f;
	}


	

	glm::vec3 playerPos = camera.position;
	glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(playerPos / float(CHUNKSIZE)));


	//achar chunk pela posicao
	/*
	glm::ivec3 pos = glm::ivec3(0, 0, 0);
	chunks[pos] = Chunk();


- [ ] Verificar se mudou de chunk (comparar com `lastPlayerPos`)
- [ ] Identificar chunks ao redor do jogador que precisam ser gerados
- [ ] Inserir esses chunks na `chunkQueue` (se ainda não existirem)
- [ ] Gerar alguns chunks da `chunkQueue` por frame
- [ ] Armazenar chunks gerados em `WorldData`
- [ ] Renderizar todos os chunks que estão prontos em worldData (dentro da render distance)
- [ ] (Opcional) Remover chunks distantes

	*/
	

	


		


}

void World::pushChunkData() {
	

}