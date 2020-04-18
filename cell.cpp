#include <ostream>
#include "cell.h"

Cell::Cell() : clusterId(0) {

}

Cell::~Cell() {

}

unsigned long Cell::getClusterId() const {
    return clusterId;
}

void Cell::setClusterId(unsigned long clusterId) {
    this->clusterId = clusterId;
}

void Cell::removeClusterId() {
    clusterId = 0;
}

std::ostream& operator <<(std::ostream& stream, const Cell* item) {
    return stream << "Cell<" << item->getClusterId() << ", " << static_cast<const void*>(item) << ">";
}
