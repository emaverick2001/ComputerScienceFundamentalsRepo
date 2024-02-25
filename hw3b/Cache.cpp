#include "Cache.h"
#include "Set.h"
#include <iostream>
// #include <math.h>

using std::cout;
using std::endl;

//Constructor
Cache::Cache(int cacheSize, int setSize, int blockSize) : cacheSize(cacheSize), setSize(setSize), blockSize(blockSize) {
    // create sets
    for (int i = 0; i < cacheSize; i++) {
        Set s = Set(setSize, blockSize);
        sets.push_back(s);
    }
}


int Cache::log2(int value){
    int result = 0;
    while (value >>= 1) result++; 
    return result;
}

void Cache::getTagAndIndex(uint32_t address, uint32_t &tag, uint32_t &index) {

    int blockOffsetBits = log2(blockSize); // offset bits
    int setBits = log2(cacheSize); // index bits

    index = (address >> blockOffsetBits) & ((1 << setBits) - 1);
    tag = address >> (blockOffsetBits + setBits);
}