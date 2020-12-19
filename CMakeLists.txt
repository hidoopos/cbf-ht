cmake_minimum_required(VERSION 3.13)
project(FIB0331)

set(CMAKE_CXX_STANDARD 11)

add_executable(FIB main.cpp NDN-PrefixGenerator.cpp NDN-HashBasedFIB.cpp city-hash.cpp hashFuncs.cpp BloomFilter.cpp CBF-HT.cpp hashtable.cpp BinarySearch.cpp)