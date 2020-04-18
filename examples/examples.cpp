#include <vector>
#include <map>
#include <thread>
#include <sstream>
#include <unistd.h>
#include "../src/clustergroup.h"
#include "../src/clustermap.h"
#include "./examples.h"
#include "./cell.h"


namespace Smoren::ThreadContainers::Examples {
    void testClusterGroup() {
        showTitle("BEGIN: testClusterGroup");
        ClusterGroup<Cell> cg(4);

        cg.setHandler([](ClusterGroup<Cell>& cg, Cluster<Cell>& cluster) {
            unsigned long id = cluster.getId();

            while(!cg.isTerminated()) {
                cg.log(id, "lock storage 1");
                for(unsigned long i=0; i<500000; i++) {
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

        std::vector<std::thread> threads = cg.run();

        sleep(10);
        cg.terminate();
        cg.log(0, "TERMINATE SIGNAL");

        for(auto& thread : threads) {
            thread.join();
        }

        //std::cout << cg << std::endl;
        std::cout << cg.getItemsCount() << std::endl;
        end = std::chrono::system_clock::now();
        std::cout << "time spent: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
        showTitle("END: testClusterGroup");
        std::cout << endl;
    }

    void testClusterMap() {
        showTitle("BEGIN: testClusterMap");
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();

        ClusterMap<Cell*> m(10, 10000);

        for(int i=0; i<10; i++) {
            vector<thread> threads;

            for(int i=0; i<10; i++) {
                threads.push_back(thread(threadClusterMap, ref(m), i, i*10000, i*10000+10000));
                //threads.push_back(thread(threadClusterMap, ref(m), i, 10000, 20000));
            }

            for(auto& th : threads) {
                th.join();
            }

            cout << "step " << i << endl;
        }

        //cout << m << endl;
        end = std::chrono::system_clock::now();
        std::cout << "time spent: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
        showTitle("END: testClusterMap");
        std::cout << endl;
    }

    void testClusterMapBenchWithStdMap() {
        showTitle("BEGIN: testClusterMapBenchWithStdMap");
        std::map<int, Cell*> simpleMap({});
        ClusterMap<Cell*> clusterMap(10000, 1000);

        {
            std::chrono::time_point<std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();
            for(unsigned long i=0; i<10000000; i++) {
                simpleMap.insert({i, new Cell(i)});
            }
            //cout << simpleMap.size() << endl;
            end = std::chrono::system_clock::now();
            std::cout << "simple map mass insert: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
        }

        {
            std::chrono::time_point<std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();
            for(unsigned long i=0; i<10000000; i++) {
                clusterMap.insert({i, new Cell(i)});
            }
            //cout << clusterMap.size() << endl;
            end = std::chrono::system_clock::now();
            std::cout << "cluster map mass insert: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
        }

        {
            std::chrono::time_point<std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();
            for(auto& p : simpleMap) {}
            end = std::chrono::system_clock::now();
            std::cout << "simple map iterating: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
        }

        {
            std::chrono::time_point<std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();
            for(auto& p : clusterMap) {}
            end = std::chrono::system_clock::now();
            std::cout << "cluster map iterating: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
        }
        showTitle("END: testClusterMapBenchWithStdMap");
        std::cout << endl;
    }

    void showTitle(std::string message) {
        for(unsigned long i=0; i<message.size(); i++) {
            std::cout << "=";
        }
        std::cout << std::endl << message << std::endl;
        for(unsigned long i=0; i<message.size(); i++) {
            std::cout << "=";
        }
        std::cout << endl;
    }

    void threadClusterMap(ClusterMap<Cell*>& m, unsigned long id, unsigned long from, unsigned long until) {
        for(unsigned long i=from; i<until; i++) {
            m.insert({i, new Cell(id)});
            std::this_thread::yield();
        }
    }
}
