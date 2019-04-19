#ifndef EXTMEMORY_QUICKSELECT_H
#define EXTMEMORY_QUICKSELECT_H

#include <random>
#include "TSelectionAlgorithm.h"

template <typename T>
class QuickSelect : public TSelectionAlgorithm<T> {
public:
    // Returns value at position
    T Select(TFileReader<T>& reader, uint64_t position, uint64_t memoryLimit, const std::string& fileName) {
        reader.Open();

        uint64_t size = reader.GetNumber();

        if (size <= memoryLimit) {

            TMemoryBlock<T> fullLoad = std::move(reader.ReadFirst(size));

            return fullLoad.GetSingle(position);
        }

        uint64_t index = mt() % size;

        T value = reader.ReadSingleFrom(index);

        TMemoryBlock<T> data;

        TFileWriter<T> writerLess(
            reader.GetBlockSize(), fileName + "L"
        ), writerGreater(
            reader.GetBlockSize(), fileName + "G"
        );

        uint64_t cntLess = 0, cntEqual = 0;
        reader.Open();

        do {
            data = reader.ReadFirst();
            for (int i = 0; i < data.GetBlockSize(); i++) {
                T x = data[i];
                if (x < value) {
                    writerLess.Write(x);
                    cntLess++;
                } else if (x > value) {
                    writerGreater.Write(x);
                } else {
                    cntEqual++;
                }
            }
        } while (data.GetBlockSize() != 0);

        writerLess.Close();
        writerGreater.Close();

        bool recursionLess = position < cntLess;
        bool recursionGreater = position >= cntLess + cntEqual;
        T result;

        std::string nextFileName = fileName + (recursionLess ? "L" : "G");

        TFileReader<T> nextFileReader(reader.GetBlockSize(), nextFileName);

        if (recursionLess) {
            result = Select(nextFileReader, position, memoryLimit, nextFileName);
        } else if (recursionGreater) {
            result = Select(nextFileReader, position - cntLess - cntEqual, memoryLimit, nextFileName);
        } else
            result = value;

        nextFileReader.Close();
        remove((fileName + "L").c_str());
        remove((fileName + "G").c_str());

        return result;
    }
private:
    std::mt19937_64 mt;
};

#endif //EXTMEMORY_QUICKSELECT_H
