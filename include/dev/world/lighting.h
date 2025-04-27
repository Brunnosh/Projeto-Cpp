#pragma once
#include <utility>
#include <unordered_map>
#include <hash.h>
#include <world.h>

class Renderer;

namespace Lighting {
    extern std::stack<std::pair<int,int>> pendingColumns;
    extern std::set<std::pair<int,int>> columnsPendingControl;

    void queueColumnForLightingUpdate(int x, int z);
    bool checkChunkColumn(World& world, const std::pair<int, int>& xz);
    void processPendingColumns(World& world, Renderer& worldRenderer);
    void initializeLightColumn(World& world, const std::pair<int,int>& xz, Renderer& worldRenderer);


}