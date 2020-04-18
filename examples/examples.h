#pragma once

#include "src/clustermap.h"
#include "examples/cell.h"


namespace Smoren::ThreadContainers::Examples {
    void testClusterGroup();
    void testClusterMap();
    void testClusterMapBenchWithStdMap();
    void showTitle(std::string message);
    void threadClusterMap(ClusterMap<Cell*>& m, unsigned long id, unsigned long from, unsigned long until);
}
