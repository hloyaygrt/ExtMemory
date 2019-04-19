#ifndef EXTMEMORY_MEDIANOFMEDIANS_H
#define EXTMEMORY_MEDIANOFMEDIANS_H

#include "TSelectionAlgorithm.h"

template <typename T>
class MedianOfMedians : public TSelectionAlgorithm<T> {
public:
    // Returns value at position
    T Select(TFileReader<T>& reader, uint64_t position, uint64_t memoryLimit, const std::string& fileName) {
        // FIXME: non implemented
        return 0;
    }
};

#endif //EXTMEMORY_MEDIANOFMEDIANS_H
