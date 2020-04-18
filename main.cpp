#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <unistd.h>
#include "printer.h"
#include "cell.h"
#include "clustergroup.h"

using namespace std;
using namespace Smoren::ThreadContainers;

int main()
{
    ClusterGroup< Cluster<Cell> > cg(4);

    cg.setHandler([](ClusterGroup< Cluster<Cell> >& cg, Cluster<Cell>& cluster) {
        unsigned long id = cluster.getId();

        while(!cg.isTerminated()) {
            cg.log(id, "lock storage 1");
            for(unsigned long i=0; i<1000000; i++) {
                cg.add(new Cell());
            }
            cg.log(id, "unlock storage 1");

            cg.finishPhaseBuffering();

            cg.log(id, "start apply 1");
            cluster.apply();
            cg.log(id, "finish apply 1");

            cg.finishPhaseApplying();

            cg.log(id, "lock storage 2");
            unsigned long i=0;
            for(auto* cell : cluster.getStorage()) {
                if(i++ % 2) {
                    cluster.remove(cell);
                }
            }
            cg.log(id, "unlock storage 2");

            cg.finishPhaseBuffering();

            cg.log(id, "start apply 2");
            cluster.apply();
            std::stringstream ss;
            ss << "finish apply 2 (total in cluster - " << cluster.getStorage().size() << ")";
            cg.log(id, ss.str());

            cg.finishPhaseApplying();
        }
    });

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<thread> threads = cg.run();

    sleep(10);
    cg.terminate();
    cg.log(0, "TERMINATE SIGNAL");

    for(auto& thread : threads) {
        thread.join();
    }

    cout << cg.getItemsCount() << endl;
    end = std::chrono::system_clock::now();
    std::cout << "time spent: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;

    return 0;
}
