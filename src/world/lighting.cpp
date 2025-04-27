#include <lighting.h>


namespace Lighting {
    std::stack<std::pair<int, int>> pendingColumns;
    std::set<std::pair<int, int>> columnsPendingControl;

    void queueColumnForLightingUpdate(int x, int z) {
        std::pair xzCoord = std::pair(x, z);    

        if (columnsPendingControl.find(xzCoord) == columnsPendingControl.end()) {
            pendingColumns.push(xzCoord);
            Lighting::columnsPendingControl.insert(xzCoord);
        }
        
    }


}