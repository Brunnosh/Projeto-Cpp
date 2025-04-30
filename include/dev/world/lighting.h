#pragma once
#include <utility>
#include <unordered_map>
#include <hash.h>
#include <world.h>
#include <threadPool.h>
class Renderer;

namespace Lighting {
    extern std::queue<std::pair<int,int>> pendingColumns;
    extern std::set<std::pair<int,int>> columnsPendingControl;
    extern std::mutex lightMutex;

    void queueColumnForLightingUpdate(int x, int z);
    bool checkChunkColumn(World& world, const std::pair<int, int>& xz);
    void processPendingColumns(World& world, Renderer& worldRenderer, ThreadPool& pool);
    void initializeLightColumn(World& world, const std::pair<int,int>& xz, Renderer& worldRenderer);


}