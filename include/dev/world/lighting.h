#pragma once
#include <utility>
#include <unordered_map>
#include <hash.h>
#include <world.h>

namespace Lighting {
    extern std::stack<std::pair<int,int>> pendingColumns;
    extern std::set<std::pair<int,int>> columnsPendingControl;

    void queueColumnForLightingUpdate(int x, int z);

}