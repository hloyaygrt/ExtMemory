#ifndef EXTMEMORY_TFILEWRITER_H
#define EXTMEMORY_TFILEWRITER_H

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include "TMemoryBlock.h"

template<typename T>
class TFileWriter {
public:
    explicit TFileWriter(const std::string& filePath) : blockSize(0) {
        buffer = NULL;
        bufferPosition = 0;
        fileHandle = Open(filePath);
    }

    TFileWriter(size_t blockSize, const std::string& filePath): blockSize(blockSize) {
        buffer = new T[blockSize];
        bufferPosition = 0;
        fileHandle = Open(filePath);
    }

    void Write(const T& single) {
        buffer[bufferPosition++] = single;
        TryDrop();
    }

    static clock_t totalTime;

    void WriteMany(const TMemoryBlock<T>& memoryBlock) {
        clock_t start = clock();
        Output(memoryBlock.GetData(), memoryBlock.GetBlockSize());
        clock_t finish = clock();
        totalTime += finish - start;
    }

    void Close() {
        HardDrop();
        delete[] buffer;
        buffer = 0;
        fsync(fileHandle);
    }

    ~TFileWriter() {
        if (buffer != NULL)
            Close();
    }

    void HardDrop() {
        if (bufferPosition != 0) {
            Output(buffer, bufferPosition);
            bufferPosition = 0;
        }
    }
private:
    int Open(const std::string& filePath) {
        int fileHandle = open(filePath.c_str(), O_RDWR | O_CREAT, 0644);
        assert(fileHandle != -1);
        return fileHandle;
    }

    void Output(T* data, size_t count) {
        write(fileHandle, (char*)data, count * sizeof(T));
        fsync(fileHandle);
    }

    void TryDrop() {
        if (bufferPosition == blockSize) {
            HardDrop();
        }
    }

    T* buffer;
    size_t bufferPosition;
    size_t blockSize;
    int fileHandle;
};

template<typename T>
clock_t TFileWriter<T>::totalTime = 0;

#endif //EXTMEMORY_TFILEWRITER_H
