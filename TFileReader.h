#ifndef EXTMEMORY_TFILEREADER_H
#define EXTMEMORY_TFILEREADER_H

#include <string>
#include <fcntl.h>
#include <unistd.h>

#include "TMemoryBlock.h"

template<typename T>
class TFileReader {
public:
    TFileReader() = default;

    TFileReader(size_t blockSize, const std::string& filePath) : blockSize(blockSize) {
        fileHandle = open(filePath.c_str(), O_RDONLY, 0644);
        assert(fileHandle != -1);
    }

    static clock_t totalTime;

    TMemoryBlock<T> ReadFirst(size_t limit = 0) {
        if (limit == 0)
            limit = blockSize;

        char* bytes = new char[limit * sizeof(T)];

        clock_t start = clock();

        long count = read(fileHandle, bytes, limit * sizeof(T));

        clock_t finish = clock();
        totalTime += finish - start;

        assert(count != -1);
        count /= sizeof(T);

        if (count == 0) {
            delete[] bytes;
            return TMemoryBlock<T>();
        }

        T* data = new T[count];

        memcpy(data, bytes, count * sizeof(T));

        delete[] bytes;

        TMemoryBlock<T> result = TMemoryBlock<T>((size_t)count, data, false);
        return result;
    }

    T ReadSingleFrom(uint64_t index) {
        clock_t start = clock();
        lseek(fileHandle, index * sizeof(T), SEEK_SET);
        T result;
        ssize_t number = read(fileHandle, (char*)&result, sizeof(T));
        clock_t finish = clock();
        totalTime += finish - start;
        assert(number == sizeof(T));
        return result;
    }

    uint64_t GetNumber() {
        clock_t start = clock();

        long long startFile = lseek(fileHandle, 0, SEEK_SET);
        long long endFile = lseek(fileHandle, 0, SEEK_END);

        clock_t finish = clock();
        totalTime += finish - start;

        Open();
        return (endFile - startFile) / sizeof(T);
    }

    void Open() {
        clock_t start = clock();

        lseek(fileHandle, 0, SEEK_SET);

        clock_t finish = clock();
        totalTime += finish - start;
    }

    void Close() {
        clock_t start = clock();

        fsync(fileHandle);

        clock_t finish = clock();
        totalTime += finish - start;
    }

    ~TFileReader() {
        Close();
    }

    size_t GetBlockSize() {
        return blockSize;
    }
private:
    size_t blockSize;
    int fileHandle;
};

template <typename T>
clock_t TFileReader<T>::totalTime = 0;

#endif //EXTMEMORY_TFILEREADER_H
