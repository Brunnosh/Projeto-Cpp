#include <world.h>


float saveTimer = 0.0f;
float saveFrequency = 30.0f;

World::World(Camera& camera){
	this->lastPlayerPos = glm::vec3(0.0f, 17.0f, 0.0f); // no futuro ler do arquvi do mundo
	

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
	


	for (int x = -camera.renderDist; x <= camera.renderDist; x++)
	{
		for (int y = 1; y <= 1; y++) 
		{
			for (int z = -camera.renderDist; z <= camera.renderDist; z++)
			{

				glm::ivec3 offset(x, 0, z);
				glm::ivec3 chunkWorldPos = playerChunkPos + offset;
				chunkWorldPos.y = 0;


				bool inWorld = WorldData.find(chunkWorldPos) != WorldData.end();
				bool alreadyQueued = chunkRequested.find(chunkWorldPos) != chunkRequested.end();

				if (!inWorld && !alreadyQueued) {
					chunkQueue.push_back(chunkWorldPos);
					chunkRequested.insert(chunkWorldPos);
				}



			}
		}
	}

	int chunksPerFrame = 4;

	int generated = 0;
	while (!chunkQueue.empty() && generated < chunksPerFrame) {
		glm::ivec3 pos = chunkQueue.back();
		chunkQueue.pop_back();

		std::future<std::pair<glm::ivec3, Chunk>> fut = std::async(std::launch::async, [pos, this]() -> std::pair<glm::ivec3, Chunk> {
			Chunk chunk(pos); // gera usando posição no mundo
			chunk.genChunk();
			return { pos, std::move(chunk) };
			});

		chunkFutures.push_back(std::move(fut));
		generated++;
	}

	for (int i = 0; i < chunkFutures.size(); ) {
		auto& fut = chunkFutures[i];
		if (fut.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto [pos, chunk] = fut.get();
			WorldData.emplace(pos, std::move(chunk));
			chunkFutures.erase(chunkFutures.begin() + i); // remove futuro concluído
		}
		else {
			++i;
		}
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