#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <unistd.h>
#include "./examples/examples.h"

using namespace std;
using namespace Smoren::ThreadSafeContainers;
using namespace Smoren::ThreadSafeContainers::Examples;

int main()
{
    testClusterGroup();
//    testClusterMap();
//    testClusterMapBenchWithStdMap();

    showTitle("ALL FINISHED!");

    return 0;
}
