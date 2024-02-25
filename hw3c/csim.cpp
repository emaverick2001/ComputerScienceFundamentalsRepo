#include <string.h>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "Cache.h"
#include "Sim.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::isdigit;
using std::istringstream;
using std::stoi;
using std::stol;
using std::string;

bool isValidInt(const char* str) {
    while (*str) {
        if (!isdigit(*str)) {
            return false;
        }
        str++;
    }
    return true;
}
bool isPowerOfTwo(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}
bool isValidParameters(int argc, char** argv) {
    /**
     * Reading Cache Parameters:
     *
     * argv[1]  - number of sets in the cache (a positive power-of-2)
     * argv[2]  - number of blocks in each set (a positive power-of-2)
     * argv[3]  - number of bytes in each block (a positive power-of-2, at least 4)
     * argv[4]  - write-allocate or no-write-allocate
     * argv[5]  - write-through or write-back
     * argv[6]  - lru (least-recently-used) or fifo evictions
     *
     * example: ./csim 256 4 16 write-allocate write-back lru  < sometracefile
     */

    // if there are not 6 cache design parameters, exit with code 1
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <cache size> <block size> <set associative> <write policy> <replacement policy> <trace file>\n";
        return false;
    }

    // Cache size validation: must be a positive power of 2
    char* numSets = argv[1];
    if (!isValidInt(numSets)) {
        cerr << "Number of sets in a cache must be a positive power-of-2" << endl;
        return false;
    }
    int cache_size = stoi(numSets);
    if (!isPowerOfTwo(cache_size) || cache_size <= 0) {
        cerr << "Number of sets in a cache must be a positive power-of-2" << endl;
        return false;
    }

    // Set size validation: must be a positive power-of-2
    char* numBlocks = argv[2];
    if (!isValidInt(numBlocks)) {
        cerr << "Number of blocks in a each set must be a positive power-of-2" << endl;
        return false;
    }
    int set_size = stoi(numBlocks);
    if (!isPowerOfTwo(set_size) || set_size <= 0) {
        cerr << "Number of blocks in a each set must be a positive power-of-2" << endl;
        return false;
    }

    // Block size validation: must be a positive power-of-2 and at least 4
    char* numBytes = argv[3];
    if (!isValidInt(numBytes)) {
        cerr << "Number of bytes in each block must be a positive power-of-2" << endl;
        return false;
    }
    int block_size = stoi(numBytes);
    if (!isPowerOfTwo(block_size) || block_size <= 0) {
        cerr << "Number of bytes in each block must be a positive power-of-2" << endl;
        return false;
    }
    if (block_size < 4) {
        cerr << "Block size must be at least four" << endl;
        return false;
    }

    // MissPolicy validation
    char* cacheMissPolicy = argv[4];
    bool write_allocate;
    if (strcmp(cacheMissPolicy, "write-allocate") == 0) {
        write_allocate = true;
    } else if (strcmp(cacheMissPolicy, "no-write-allocate") == 0) {
        write_allocate = false;
    } else {
        cerr << "4th parameter must be write-allocate or no-write-allocate" << endl;
        return false;
    }

    // HitPolicy validation
    char* cacheHitPolicy = argv[5];
    bool write_type;
    if (strcmp(cacheHitPolicy, "write-through") == 0) {
        write_type = true;
    } else if (strcmp(cacheHitPolicy, "write-back") == 0) {
        write_type = false;
    } else {
        cerr << "5th parameter must be write_through or write-back" << endl;
        return false;
    }

    // Evict type validation
    char* cacheEvictPolicy = argv[6];
    if (strcmp(cacheEvictPolicy, "lru") != 0 && strcmp(cacheEvictPolicy, "fifo") != 0) {
        cerr << "6th parameter must be lru or fifo" << endl;
        return false;
    }

    // no-write-allocate cannot be combined with write-back
    if (!write_allocate && !write_type) {
        cerr << "write-back and no-write-allocate cannot both be specified" << endl;
        return false;
    }

    // passed all validation tests
    return true;
}

int main(int argc, char* argv[]) {
    // check if args are valid
    if (!isValidParameters(argc, argv)) {
        return 1;
    }

    int numSets = std::atoi(argv[1]);
    /* num of blocks per set */
    int numBlocks = std::atoi(argv[2]);
    /* num of bytes per block => block size (bytes) => 2^#offsetbits */
    int numBytes = std::atoi(argv[3]);

    std::string writeMissPolicy = argv[4];
    std::string writeHitPolicy = argv[5];
    std::string evictPolicy = argv[6];

    // Convert std::str to char *
    char *writeHitPolicyStr = new char[writeHitPolicy.length() + 1];
    strcpy(writeHitPolicyStr, writeHitPolicy.c_str());

    char *writeMissPolicyStr = new char[writeMissPolicy.length() + 1];
    strcpy(writeMissPolicyStr, writeMissPolicy.c_str());

    char *evictPolicyStr = new char[evictPolicy.length() + 1];
    strcpy(evictPolicyStr, evictPolicy.c_str());

    Cache cache = Cache(numSets, numBlocks, numBytes);

    Sim simulator = Sim(cache);

    // read each line of the trace file
    string line;
    while (getline(cin, line)) {

        istringstream iss(line);

        // read the first two fields (ignoring the third)
        char operation;  // either 'l' (loading) or 's' (storing)
        string address;  // 32-bit memory address given in hexadecimal

        iss >> operation;
        iss >> address;

        if (iss.fail()) {
            std::cerr << "Error parsing the input line." << std::endl;
            continue;  // Skip the line and continue with the next one // dont know if this should be the behavior or just print stderror
        }

        uint32_t addressInt = stol(address, 0, 16);

        simulator.run(operation, addressInt, writeMissPolicyStr , writeHitPolicyStr, evictPolicyStr);
    }
    simulator.simSummary();
    return 0;
}