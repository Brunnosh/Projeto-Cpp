#pragma once
#include <utility>
#include <unordered_map>
#include <hash.h>
#include <world.h>

namespace Lighting {
	void initializeChunkLight(const std::unordered_map<std::pair<int, int>, int, PairHash> & highestChunkY, const std::unordered_map<glm::ivec3, chunkObject, Vec3Hash> & worldData);


}