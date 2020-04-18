#pragma once

#include <ostream>
#include "src/clustergroup.h"

namespace Smoren::ThreadContainers::Examples {
    class Cell {
    public:
        Cell();
        Cell(unsigned long id);
        virtual ~Cell();
        virtual unsigned long getClusterId() const;
        virtual unsigned long getId() const;
        virtual void setClusterId(unsigned long clusterId);
        virtual void removeClusterId();

    protected:
        unsigned long clusterId;
        unsigned long id;
    };

    std::ostream& operator <<(std::ostream& stream, const Cell* item);
}
