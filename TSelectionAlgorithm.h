#ifndef EXTMEMORY_TSELECTIONALGORITHM_H
#define EXTMEMORY_TSELECTIONALGORITHM_H

#include "TFileReader.h"

template<typename T>
class TSelectionAlgorithm {
public:
    virtual T Select(TFileReader<T>& reader, uint64_t position, uint64_t memoryLimit, const std::string& fileName) = 0;
};


#endif //EXTMEMORY_TSELECTIONALGORITHM_H
