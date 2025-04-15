#include <world.h>


float saveTimer = 0.0f;
float saveFrequency = 30.0f;


World::World(Camera& camera){
	this->lastPlayerPos = glm::vec3(0.0f, 62.0f, 0.0f); // no futuro ler do arquvi do mundo
	

	camera.position = lastPlayerPos;
}


void World::update(Camera& camera,float deltaTime, unsigned int modelLoc) {

	savePlayerPos(deltaTime, camera);
	genWorld(camera, modelLoc);

	
	


}

void World::tick() {
	//In-world ticking (entities, blockstates, essentially game speed)



}



void World::savePlayerPos(float deltaTime, Camera& camera) {
	saveTimer += deltaTime;

	if (saveTimer >= saveFrequency) {
		lastPlayerPos = camera.getCamPos();
		saveTimer = 0.0f;
	}
}

void World::genWorld(Camera& camera, unsigned int modelLoc) {
	glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(camera.position / float(CHUNKSIZE)));



	for (int x = -camera.renderDist; x <= camera.renderDist; x++)
	{
		for (int z = -camera.renderDist; z <= camera.renderDist; z++)
		{
			for (int y = playerChunkPos.y - camera.renderDist / 2; y <= playerChunkPos.y + camera.renderDist; y++)
			{

				//position of chunk on the render distance loop
				glm::ivec3 offset(x, y, z);

				//translation of chunk pos from render distance loop to actual world position using player position.
				glm::ivec3 chunkWorldPos = glm::ivec3(playerChunkPos.x, 0, playerChunkPos.z) + offset;



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
			chunk.genChunkFaces(this->WorldData);
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

}

int World::getNumberChunks() {
	return WorldData.size();
}