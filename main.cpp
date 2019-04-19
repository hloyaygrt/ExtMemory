#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory.h>
#include <algorithm>

#include "TMemoryBlock.h"
#include "TFileReader.h"
#include "TFileWriter.h"
#include "TDistributionSort.h"
#include "QuickSelect.h"

using namespace std;

void ReportError(int argc, char** argv) {
    cerr << "Incorrect number of arguments: " << argc << ", expected >=3" << endl;
    cerr << "Usage: <inputPath> <outputPath> [--ml <memoryLimit>] [--bs <blockSize>]" << endl;
    exit(1);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        ReportError(argc, argv);
    }

    double start = clock();
    std::cout << "Let's go" << std::endl;

    string inputPath = argv[1];
	string outputPath = argv[2];
    size_t memoryLimit = (1 << 20) * 200;
    size_t blockSize = (1 << 20);

    if (argc > 3) {
        if (std::string(argv[3]) != "--ml")
            ReportError(argc, argv);
        memoryLimit = (size_t) std::atoll(argv[4]);
    }

    if (argc > 5) {
        if (std::string(argv[5]) != "--bs")
            ReportError(argc, argv);
        blockSize = (size_t) std::atoll(argv[6]);
    }

    QuickSelect<uint64_t> selector;

    TDistributionSort<uint64_t> sorter(
            blockSize, memoryLimit / sizeof(uint64_t), inputPath, outputPath
    );

    sorter.Sort(selector);

    std::cout << "blockSize = " << (double)blockSize / 1024. / 1024. << ' ' << " memoryLimit = " << (double) memoryLimit / 1024. / 1024. << std::endl;
    std::cout << "Total time = " << ((double)clock() - start) / CLOCKS_PER_SEC << std::endl;
    std::cout << "IO time = " << ((double)TFileReader<uint64_t>::totalTime + TFileWriter<uint64_t>::totalTime) / CLOCKS_PER_SEC << std::endl;
    return 0;
}
