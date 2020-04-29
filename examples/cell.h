#pragma once

#include <ostream>
#include "../src/clustergroup.h"

namespace Smoren::ThreadSafeContainers::Examples {
    class Cell : public Smoren::ThreadSafeContainers::ClusterItem {
    public:
        Cell();
        Cell(unsigned long id);
        virtual ~Cell();
        virtual unsigned long getClusterId() const;
        virtual void setClusterId(unsigned long clusterId);
        virtual void removeClusterId();
        virtual unsigned long getId() const;

    protected:
        unsigned long clusterId;
        unsigned long id;
    };

    std::ostream& operator <<(std::ostream& stream, const Cell* item);
}
