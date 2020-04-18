#pragma once

#include <ostream>
#include "clustergroup.h"


class Cell {
public:
    Cell();
    virtual ~Cell();
    virtual unsigned long getClusterId() const;
    virtual void setClusterId(unsigned long clusterId);
    virtual void removeClusterId();

protected:
    unsigned long clusterId;
};

std::ostream& operator <<(std::ostream& stream, const Cell* item);

