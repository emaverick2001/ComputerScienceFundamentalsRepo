#include <bitset>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Cache.h"

bool validate_parameters(int argc, char **argv);
bool isValidInt(std::string address);
bool isPowerOfTwo(int n);
bool isValidAddress(std::string address);
bool isValidHexCharacter(char character);

int main(int argc, char *argv[]) {

  // check if args are valid
  if (!validate_parameters(argc, argv)) {
    return 1;
  }

  // Initialize parameters

  /* num of sets in cache => total num slots/cache -> num sets/cache * num
   * blocks/set => 2^#indexbits*/
  int numSets = std::atoi(argv[1]);
  /* num of index bits => form index into cache */
  int numIndexBits = std::log2(numSets);
  /* num of blocks per set */
  int numBlocks = std::atoi(argv[2]);
  /* block size (bytes) */
  int blockSize = std::atoi(argv[3]);
  /* num offset bits */
  int numOffsetBits = std::log2(blockSize);
  /* num tag bits */
  int numTagBits = 32 - numIndexBits - numOffsetBits;

  std::string writeMissPolicy = argv[4];
  std::string writeHitPolicy = argv[5];
  std::string evictPolicy = argv[6];

  // Initializing counters
  int totalLoads = 0;
  int totalStores = 0;
  int loadHits = 0;
  int loadMisses = 0;
  int storeHits = 0;
  int storeMisses = 0;
  int totalCycles = 0;

  // Initialize cache
  Cache cache = Cache(numSets, numBlocks, blockSize);

  // read file from cin
  std::string line;

  while (std::getline(std::cin, line)) {

    // parsing each line from input
    std::cout << line << "\n";
    char operation;
    std::string address;

    // Parse the input line
    std::istringstream iss(line);
    iss >> operation >> address;
    uint32_t addressInt = stol(address, 0, 16);
    std::cout << operation << "\n";
    std::cout << address << "\n";

    if (iss.fail()) {
      std::cerr << "Error parsing the input line." << std::endl;
      continue; // Skip the line and continue with the next one // dont know if
                // this should be the behavior or just print stderror
    }

    // error checking for operation (needs to be s or l)
    if (operation != 's' || operation != 'l') {
      std::cerr << "Invalid operation" << std::endl;
      return 1;
    }

    // error checking for address
    if (!isValidAddress(address)) {
      std::cerr << "Invalid address" << std::endl;
      return 1;
    }

    

    




  }
}

bool validate_parameters(int argc, char **argv) {
  /**
   * Cache Parameters + input info:
   *
   * argv[1]  - numSets = number of sets in the cache (a positive power-of-2)
   * argv[2]  - numBlocks = number of blocks in each set (a positive power-of-2)
   * argv[3]  - numBytes = number of bytes in each block (a positive power-of-2,
   * at least 4) argv[4]  - write-allocate or no-write-allocate argv[5]  -
   * write-through or write-back argv[6]  - lru (least-recently-used) or fifo
   * evictions
   *
   * example: ./csim 256 4 16 write-allocate write-back lru  < sometracefile
   */
  if (argc != 7) {
    std::cerr << "Usage: " << argv[0]
              << " <cache size> <block size> <set associative> <write policy> "
                 "<replacement policy> <trace file>\n";
    return false;
  }

  std::string numSets = "";
  std::string numBlocks = "";
  std::string numBytes = "";
  std::string cacheHitPolicy = "";
  std::string cacheMissPolicy = "";
  std::string cacheEvictPolicy = "";
  int cache_size = 0;
  int set_size = 0;
//   int block_size = 0;

  // Cache size validation: must be a positive power of 2
  numSets = argv[1];
  if (!isValidInt(numSets)) {
    std::cerr << "Number of sets in a cache must be a positive power-of-2"
              << std::endl;
    return false;
  }

  cache_size = std::stoi(argv[1]);
  if (!isPowerOfTwo(cache_size) || cache_size <= 0) {
    std::cerr << "Number of sets in a cache must be a positive power-of-2"
              << std::endl;
    return false;
  }

  // Set size/ num Blocks validation: must be a positive power-of-2
  numBlocks = argv[2];
  if (!isValidInt(numBlocks)) {
    std::cerr << "Number of blocks in a each set must be a positive power-of-2"
              << std::endl;
    return false;
  }
  set_size = std::stoi(argv[2]);
  if (!isPowerOfTwo(set_size) || set_size <= 0) {
    std::cerr << "Number of blocks in a each set must be a positive power-of-2"
              << std::endl;
    return false;
  }

  // Block size validation: must be a positive power-of-2 and at least 4
  numBytes = argv[3];
  if (!isValidInt(numBytes)) {
    std::cerr << "Number of bytes in each block must be a positive power-of-2"
              << std::endl;
    return false;
  }

  int block_size = std::stoi(argv[3]);

  if (!isPowerOfTwo(block_size) || block_size <= 0) {
    std::cerr << "Number of bytes in each block must be a positive power-of-2"
              << std::endl;
    return false;
  }
  if (block_size < 4) {
    std::cerr << "Block size must be at least four" << std::endl;
    return false;
  }

  // MissPolicy validation
  cacheMissPolicy = argv[4];
  bool MissPolicyValid = false;
  if (cacheMissPolicy == "write-allocate") {
    MissPolicyValid = true;
  } else if (cacheMissPolicy == "no-write-allocate") {
    MissPolicyValid = true;
  } else {
    MissPolicyValid = false;
  }

  if (!MissPolicyValid) {
    std::cerr << "4th parameter must be write-allocate or no-write-allocate"
              << std::endl;
    return false;
  }

  // HitPolicy validation
  cacheHitPolicy = argv[5];
  bool HitPolicyValid = false;
  if (cacheHitPolicy == "write-through") {
    HitPolicyValid = true;
  } else if (cacheMissPolicy == "write-back") {
    HitPolicyValid = true;
  } else {
    HitPolicyValid = false;
  }

  if (!HitPolicyValid) {
    std::cerr << "5th parameter must be write-through or write-back"
              << std::endl;
    return false;
  }

  // Evict type validation
  cacheEvictPolicy = argv[6];
  bool evictPolicyValid = false;
  if (cacheEvictPolicy == "lru") {
    evictPolicyValid = true;
  } else if (cacheEvictPolicy == "fifo") {
    evictPolicyValid = true;
  } else {
    evictPolicyValid = false;
  }
  if (!evictPolicyValid) {
    std::cerr << "6th parameter must be lru or fifo" << std::endl;
    return false;
  }

  // no-write-allocate cannot be combined with write-back
  if (cacheMissPolicy == "no-write-allocate" &&
      cacheHitPolicy == "write-back") {
    std::cerr << "write-back and no-write-allocate cannot both be specified"
              << std::endl;
    return false;
  }

  // passed all validation tests
  return true;
}

bool isValidInt(std::string address) {
  int i = 0;
  char character = address[i];
  bool isValid = false;
  while (character != NULL) {
    if (isdigit(character)) {
      isValid = true;
    }
    isValid = false;
    i++;
  }
  return isValid;
}

bool isPowerOfTwo(int n) {
  // Check if n is greater than 0 and has exactly one bit set to 1
  return (n > 0) && ((n & (n - 1)) == 0); // should work - double check
}

bool isValidAddress(std::string address) {
  if (address.substr(0, 2) == "0x" && address.length() == 10) {
    for (int i = 0; i < 10; i++) {
      if (!isValidHexCharacter(address[i])) {
        return false;
      }
    }
    return true;
  }
  // has 10 characters
  // first 2 characters are 0x
  // rest are values 0-9 and e-f
  return false;
}

bool isValidHexCharacter(char character) {
  switch (character) {
  case 'A':
  case 'a':
  case 'B':
  case 'b':
  case 'C':
  case 'c':
  case 'D':
  case 'd':
  case 'E':
  case 'e':
  case 'F':
  case 'f':
    return true;
  default:
    return false;
  }
}