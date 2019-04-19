#ifndef EXTMEMORY_TMEMORYBLOCK_H
#define EXTMEMORY_TMEMORYBLOCK_H

#include <memory.h>
#include <iostream>
#include <algorithm>

template<typename T>
class TMemoryBlock {
public:
    TMemoryBlock() {
        data = 0;
        blockSize = 0;
    }

    TMemoryBlock(const TMemoryBlock<T>& another) {
        if (another.data != NULL) {
            blockSize = another.blockSize;
            data = new T[blockSize];
            memcpy(data, another.data, blockSize * sizeof(T));
        } else {
            blockSize = 0;
            data = 0;
        }
    }

    TMemoryBlock & operator=(const TMemoryBlock& rhs){
        if (this != &rhs) {
            TMemoryBlock tmp(rhs);
            std::swap(tmp.data, data);
            std::swap(tmp.blockSize, blockSize);
        }
        return *this;
    }

    TMemoryBlock(size_t blockSize, const T& single) : blockSize(blockSize) {
        data = new T[blockSize];
        for (size_t i = 0; i < blockSize; i++)
            data[i] = single;
    }

    TMemoryBlock(size_t blockSize, T* data, bool copy = true) : blockSize(blockSize) {
        if (copy) {
            this->data = new T[blockSize];
            memcpy(this->data, data, blockSize * sizeof(T));
        } else {
            this->data = data;
        }
    }

    ~TMemoryBlock() {
        if (data != NULL) {
            delete[] data;
            data = NULL;
            blockSize = 0;
        }
    }

    size_t GetBlockSize() const {
        return blockSize;
    }

    T operator[] (size_t i) const {
        return data[i];
    }

    T* GetData() const {
        T* result = new T[blockSize];
        memcpy(result, data, blockSize * sizeof(T));
        return result;
    }

    void SortData() {
        std::sort(data, data + blockSize);
    }

    T GetSingle(uint64_t position) {
        std::nth_element(data, data + position, data + blockSize);
        return data[position];
    }
private:
    size_t blockSize;
    T* data;
};


#endif //EXTMEMORY_TMEMORYBLOCK_H
