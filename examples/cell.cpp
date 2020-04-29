#include <ostream>
#include "./cell.h"


namespace Smoren::ThreadSafeContainers::Examples {
    Cell::Cell() : clusterId(0), id(0) {

    }

    Cell::Cell(unsigned long id) : clusterId(0), id(id) {

    }

    Cell::~Cell() {

    }

    unsigned long Cell::getClusterId() const {
        return clusterId;
    }

    unsigned long Cell::getId() const {
        return id;
    }

    void Cell::setClusterId(unsigned long clusterId) {
        this->clusterId = clusterId;
    }

    void Cell::removeClusterId() {
        clusterId = 0;
    }

    std::ostream& operator <<(std::ostream& stream, const Cell* item) {
        if(item == nullptr) {
            return stream << "null";
        }

        if(item->getId()) {
            return stream << "<Cell #" << item->getId() << ">";
        }

        return stream << "Cell<" << item->getClusterId() << ", " << static_cast<const void*>(item) << ">";
    }
}
