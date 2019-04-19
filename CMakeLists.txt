cmake_minimum_required(VERSION 3.13)
project(ExtMemory)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -detect_leaks=1 -g")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2")

add_executable(ExtMemory TFileReader.h TMemoryBlock.h TFileWriter.h TDistributionSort.h MedianOfMedians.h TSelectionAlgorithm.h QuickSelect.h main.cpp)