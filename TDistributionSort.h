#ifndef EXTMEMORY_TDISTRIBUTIONSORT_H
#define EXTMEMORY_TDISTRIBUTIONSORT_H

#include <string>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include "TFileReader.h"
#include "TFileWriter.h"
#include "MedianOfMedians.h"

template <typename T>
class TDistributionSort {
public:
    TDistributionSort(
        size_t blockSize, unsigned long long memoryLimit, const std::string& filePath,
        const std::string& outputFilePath = ""
    ) : blockSize(
            blockSize
        ), reader(
            blockSize, filePath
        ), filePath(
            filePath
        ), memoryLimit(
            memoryLimit
        ), outputFilePath(
            outputFilePath
        ) {

        if (outputFilePath.empty())
            this->outputFilePath = filePath;

        size = reader.GetNumber();
        pivotK = (size_t)std::min((uint64_t) sqrt(memoryLimit / blockSize), size / 2 + 1);
        assert(pivotK != 0);
    }

    void Sort(TSelectionAlgorithm<T>& selection) {
        TMemoryBlock<T> data;

        if (size == 0)
            return;

        if (size <= memoryLimit) {
            std::vector<T> values = InternalSort();
            reader.Close();

            TFileWriter<T> outputWriter(blockSize, outputFilePath);

            for (const T& v : values)
                outputWriter.Write(v);
            return;
        }

        std::vector<T> pivots = SelectPivots(selection);
        std::vector<uint64_t> count(pivots.size(), 0);

        for (size_t i = 0; i <= pivotK; i++) {
            bucketFiles.push_back(new TFileWriter<T>(blockSize, FilePath(i)));
        }

        reader.Open();

        assert(pivots.size() + 1 == bucketFiles.size());

        // Memory pivotK * blockSize

        do {
            data = reader.ReadFirst();

            for (size_t i = 0; i < data.GetBlockSize(); i++) {
                T value = data[i];
                size_t bucket = std::lower_bound(pivots.begin(), pivots.end(), value) - pivots.begin();
                if (bucket < pivots.size() && pivots[bucket] == value) {
                    count[bucket] += 1;
                    continue;
                }
                bucketFiles[bucket]->Write(value);
            }
        } while (data.GetBlockSize() != 0);

        for (size_t i = 0; i < bucketFiles.size(); i++)
            bucketFiles[i]->Close();
        bucketFiles.clear();
        bucketFiles.shrink_to_fit();

        // Memory clear

        reader.Close();

        TFileWriter<T> outputWriter(outputFilePath);

        uint64_t returned_data = 0;

        for (size_t i = 0; i <= pivotK; i++) {
            TDistributionSort branch(blockSize, memoryLimit, FilePath(i));
            branch.Sort(selection);

            TFileReader<T> bucketReader(blockSize, FilePath(i));

            // TODO: delete copying code
            TMemoryBlock<T> data;
            do {
                data = bucketReader.ReadFirst();
                outputWriter.WriteMany(data);
                returned_data += data.GetBlockSize();
            } while (data.GetBlockSize() != 0);

            bucketReader.Close();

            remove(FilePath(i).c_str());

            if (i < pivotK) {
                returned_data += count[i];

                while (count[i] > 0) {
                    outputWriter.WriteMany(TMemoryBlock<T>(std::min(count[i], blockSize), pivots[i]));
                    count[i] -= std::min(count[i], blockSize);
                }
            }

        }

        if (returned_data != size) {
            std::cout << filePath << std::endl;
            exit(228);
        }
    }
private:
    std::string FilePath(int i) {
        return filePath + std::to_string(i);
    }

    std::vector<T> InternalSort() {
        std::vector<T> values;
        values.reserve(size);

        TMemoryBlock<T> data;
        do {
            data = reader.ReadFirst();
            for (size_t i = 0; i < data.GetBlockSize(); i++) {
                values.push_back(data[i]);
            }
        } while (data.GetBlockSize() != 0);

        std::sort(values.begin(), values.end());

        return values;
    }

    std::vector<T> SelectPivots(TSelectionAlgorithm<T>& selection) {
        reader.Open();

        TFileWriter<T> candidatesWriter(blockSize, filePath + "candidates.out");

        for (uint64_t left = 0, right = memoryLimit - 1; left < size; left += memoryLimit, right += memoryLimit) {
            if (right > size)
                right = size - 1;

            TMemoryBlock<T> fullLoad = std::move(reader.ReadFirst(right - left + 1));
            fullLoad.SortData();

            size_t alpha = pivotK; // TODO: check this constant
            for (auto i = alpha; i < fullLoad.GetBlockSize(); i += alpha)
                candidatesWriter.Write(fullLoad[i]);
        }
        candidatesWriter.Close();

        TFileReader<T> candidatesReader(blockSize, filePath + "candidates.out");

        std::vector<T> pivots;

        uint64_t candidatesSize = candidatesReader.GetNumber();
        for (size_t j = 1; j <= pivotK; j++) {
            uint64_t position = candidatesSize / (pivotK + 1) * j;
            assert(position < candidatesSize);
            assert(position >= 0);
            T value = selection.Select(candidatesReader, position, memoryLimit, filePath + "selectionphase");
            pivots.push_back(value);
        }

        candidatesReader.Close();
        remove((filePath + "candidates.out").c_str());

        std::sort(pivots.begin(), pivots.end());
        pivots.resize(std::unique(pivots.begin(), pivots.end()) - pivots.begin());
        pivotK = pivots.size();

        return pivots;
    }

    TFileReader<T> reader;
    std::vector<TFileWriter<T>* > bucketFiles;
    size_t pivotK;
    std::string filePath, outputFilePath;
    size_t blockSize;
    unsigned long long memoryLimit, size;
};
#endif //EXTMEMORY_TDISTRIBUTIONSORT_H
