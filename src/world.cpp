#include <world.h>


float saveTimer = 0.0f;
float saveFrequency = 30.0f;

World::World(Camera& camera){
	this->lastPlayerPos = glm::vec3(0.0f, 17.0f, 0.0f); // no futuro ler do arquvi do mundo
	this->renderDist = 4;

	camera.position = lastPlayerPos;
}

//achar chunk pelo xyz 
/*
glm::ivec3 pos = glm::ivec3(0, 0, 0);
chunks[pos] = Chunk();
*/

void World::update(Camera& camera,float deltaTime, unsigned int modelLoc) {

	//Salva apenas a posicao do player, no futuro, salvar tudo do world para arquivo (chunks, player, entidades, etc)
	saveTimer += deltaTime; 

	if (saveTimer >= saveFrequency) {
		lastPlayerPos = camera.getCamPos(); 
		saveTimer = 0.0f;
	}

	//pegar XYZ do chunk onde player está
	
	glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(camera.position / float(CHUNKSIZE)));
	


	for (int x = -renderDist; x <= renderDist; x++) 
	{
		for (int y = 1; y <= 1; y++) 
		{
			for (int z = -renderDist; z <= renderDist; z++) 
			{

				glm::ivec3 offset(x, 0, z);
				glm::ivec3 chunkWorldPos = playerChunkPos + offset;
				chunkWorldPos.y = 0;
				if (WorldData.find(chunkWorldPos) == WorldData.end()) {
					chunkQueue.push_back(chunkWorldPos); // adiciona pra gerar depois
				}


			}
		}
	}

	int chunksPerFrame = 4;

	int generated = 0;
	while (!chunkQueue.empty() && generated < chunksPerFrame) {
		glm::ivec3 pos = chunkQueue.back();
		chunkQueue.pop_back();

		Chunk newChunk(pos * CHUNKSIZE); // chunk usa worldPos em blocos
		newChunk.genChunk();
		WorldData.emplace(pos, std::move(newChunk));
		generated++;
	}

	for (auto& [pos, chunk] : WorldData) {
		chunk.render(modelLoc);
	}

	//if (playerChunkPos != lastPlayerChunkPos) 

		//- [ ] Identificar chunks ao redor do jogador que precisam ser gerados

		//- [ ] Verificar se mudou de chunk e gerar novos chunks (comparar com `lastPlayerPos`)


		//- [ ] Inserir esses chunks na `chunkQueue` (se ainda não existirem)


		//- [ ] Gerar alguns chunks da `chunkQueue` por frame


		//- [ ] Armazenar chunks gerados em `WorldData`

		
		//- [ ] (Opcional) Remover chunks distantes

	


	

	


		
	//- [ ] Renderizar todos os chunks que estão prontos em worldData (dentro da render distance)

}

void World::pushChunkData() {
	

}