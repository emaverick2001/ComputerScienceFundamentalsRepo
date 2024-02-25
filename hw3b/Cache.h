#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include "Set.h"
#include <stdint.h>

using std::vector;

class Cache {
    public:
    // constructor
    Cache(int cacheSize, int setSize, int blockSize);

    // helper methods
    void getTagAndIndex(uint32_t address, uint32_t &tag, uint32_t &index);
    int log2(int value);

    // fields
    vector<Set> sets; // cache = vector of sets
    int cacheSize; // number of sets in the cache (a positive power-of-2)
    int setSize; // number of blocks in each set (a positive power-of-2)˜˜
    int blockSize; // number of bytes in each block (a positive power-of-2, at least 4)
    int accessCounter;    
};

#endif