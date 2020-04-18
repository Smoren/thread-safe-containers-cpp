#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <mutex>
#include <functional>
#include <thread>

namespace Smoren::ThreadContainers {
    template<typename ClusterItem> class Cluster {
    public:
        Cluster(unsigned long id) : clusterId(id) {

        }

        ~Cluster() {
            apply();
            for(auto* item : storage) {
                delete item;
            }
        }

        unsigned long getId() {
            return clusterId;
        }

        void add(ClusterItem* item) {
            bufAddMutex.lock();
            bufAdd.push(item);
            bufAddMutex.unlock();
        }

        void remove(ClusterItem* item) {
            bufRemoveMutex.lock();
            bufRemove.push(item);
            bufRemoveMutex.unlock();
        }

        void lockStorage() {
            storageMutex.lock();
        }

        void unlockStorage() {
            storageMutex.unlock();
        }

        void apply() {
            storageMutex.lock();

            bufRemoveMutex.lock();
            while(bufRemove.size()) {
                erase(bufRemove.top());
                bufRemove.pop();
            }
            bufRemoveMutex.unlock();

            bufAddMutex.lock();
            while(bufAdd.size()) {
                insert(bufAdd.top());
                bufAdd.pop();
            }
            bufAddMutex.unlock();

            storageMutex.unlock();
        }

        const std::set<ClusterItem*>& getStorage() const {
            return storage;
        }
    protected:
        unsigned long clusterId;
        std::set<ClusterItem*> storage;
        std::stack<ClusterItem*> bufAdd;
        std::stack<ClusterItem*> bufRemove;
        std::mutex storageMutex;
        std::mutex bufAddMutex;
        std::mutex bufRemoveMutex;

        void insert(ClusterItem* item) {
            item->setClusterId(clusterId);
            storage.insert(item);
        }

        void erase(ClusterItem* item) {
            item->removeClusterId();
            storage.erase(item);
            delete item;
        }
    };

    template<typename ClusterItem>
    std::ostream& operator <<(std::ostream& stream, const Cluster<ClusterItem>& cluster) {
        return stream << cluster.getStorage();
    }

    template<typename Cluster, typename ClusterItem> class ClusterGroup {
    public:
        const static bool PHASE_BUFFERING = false;
        const static bool PHASE_APPLYING = true;

        ClusterGroup(
            unsigned long size
        ) : size(size), currentClusterIndex(0), phase(ClusterGroup::PHASE_BUFFERING), phaseFinishCounter(0), terminated(false) {
            for(unsigned long i=0; i<size; i++) {
                clusters.push_back(new Cluster(i+1));
            }
        }

        ~ClusterGroup() {
            for(unsigned long i=0; i<size; i++) {
                delete clusters[i];
            }
        }

        void add(ClusterItem* item) {
            clusters.at(nextClusterIndex())->add(item);
        }

        void remove(ClusterItem* item) {
            unsigned long clusterId = item->getClusterId();
            checkClusterId(clusterId);
            clusters.at(item->getClusterId()-1)->remove(item);
        }

        void apply() {
            for(auto* cluster : clusters) {
                cluster->apply();
            }
        }

        void finishPhaseBuffering() {
            finishPhase();
            while(isPhaseBuffering()) {
                std::this_thread::yield();
            }
        }

        void finishPhaseApplying() {
            finishPhase();
            while(isPhaseApplying()) {
                std::this_thread::yield();
            }
        }

        const std::vector<Cluster*>& getClusters() const {
            return clusters;
        }

        std::vector<Cluster*>& getClusters() {
            return clusters;
        }

        Cluster& getCluster(unsigned long clusterId) {
            checkClusterId(clusterId);
            return *clusters.at(clusterId-1);
        }

        unsigned long getItemsCount() const {
            unsigned long result = 0;
            for(auto* cluster : clusters) {
                result += cluster->getStorage().size();
            }

            return result;
        }

        void setHandler(std::function<void(ClusterGroup<Cluster, ClusterItem>&, Cluster&)> handler) {
            this->handler = handler;
        }

        std::vector<std::thread> run() {
            std::vector<std::thread> threads;

            for(auto* cluster : getClusters()) {
                threads.push_back(std::thread(this->handler, ref(*this), ref(*cluster)));
            }

            return threads;
        }

        bool isTerminated() const {
            return terminated;
        }

        void terminate() {
            terminated = true;
        }

        void log(unsigned long clusterId, std::string message) {
            logMutex.lock();
            std::cout << clusterId << ": " << message << std::endl;
            logMutex.unlock();
        }
    protected:
        unsigned long size;
        unsigned long currentClusterIndex;
        bool phase;
        unsigned long phaseFinishCounter;
        bool terminated;
        std::function<void(ClusterGroup<Cluster, ClusterItem>&, Cluster&)> handler;
        std::mutex clusterIndexMutex;
        std::mutex phaseMutex;
        std::mutex logMutex;
        std::vector<Cluster*> clusters;

        unsigned long nextClusterIndex() {
            clusterIndexMutex.lock();
            unsigned long result = currentClusterIndex++;
            if(currentClusterIndex == size) {
                currentClusterIndex = 0;
            }
            clusterIndexMutex.unlock();
            return result;
        }

        void checkClusterId(unsigned long clusterId) {
            if(clusterId == 0 || clusterId > size) {
                throw std::runtime_error("cluster id is out of range");
            }
        }

        void finishPhase() {
            phaseMutex.lock();
            if(++phaseFinishCounter == size) {
                phaseFinishCounter = 0;
                phase = !phase;
            }
            phaseMutex.unlock();
        }

        bool isPhase(int phase) const {
            return (phase == this->phase);
        }

        bool isPhaseBuffering() const {
            return isPhase(ClusterGroup::PHASE_BUFFERING);
        }

        bool isPhaseApplying() const {
            return isPhase(ClusterGroup::PHASE_APPLYING);
        }
    };

    template<typename ClusterItem>
    std::ostream& operator <<(std::ostream& stream, const Cluster<ClusterItem>* cluster) {
        return stream << *cluster;
    }

    template<typename Cluster, typename ClusterItem>
    std::ostream& operator <<(std::ostream& stream, const ClusterGroup<Cluster, ClusterItem>& group) {
        return stream << group.getClusters();
    }
}


