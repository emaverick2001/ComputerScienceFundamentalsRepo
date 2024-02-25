#include "Sim.h"
#include <cstdint>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include "Cache.h"

using std::cout;
using std::endl;

Sim::Sim(Cache cache)
    : cache(cache),
      loads(0),
      stores(0),
      loadHits(0),
      loadMisses(0),
      storeHits(0),
      storeMisses(0),
      totalCycles(0){};

void Sim::simSummary() {
  cout << "Total loads: " << loads << endl
       << "Total stores: " << stores << endl
       << "Load hits: " << loadHits << endl
       << "Load misses: " << loadMisses << endl
       << "Store hits: " << storeHits << endl
       << "Store misses: " << storeMisses << endl
       << "Total cycles: " << totalCycles << endl;
}


void Sim::run(char operation, u_int32_t address, char* writeMissPolicy,
              char* writeHitPolicy, char* evictPolicy) {
  u_int32_t tag = 0;
  u_int32_t index = 0;
  cache.getTagAndIndex(address, tag, index); // Get tag and set index

  unsigned int setSize = cache.sets[index].slots.size();

  // get slot Index
  int slotIndex = -1;
  for (unsigned int i = 0; i < setSize; i++) {
      Slot currSlot = cache.sets[index].slots[i];
      if ((currSlot.tag == tag) && (currSlot.valid == true)) {
      slotIndex = i;
      }
  }

  if (operation == 's') {
    stores++;
    if ((slotIndex != -1) && (cache.sets[index].slots[slotIndex].valid == true)) {  // store hit
      storeHits++;

      if (strcmp(writeHitPolicy,"write-through") == 0) {  // write directly to memory, and update line in cache
        Set &s = cache.sets[index];
        totalCycles++; // accesses data in cache
        totalCycles += 100; // write to memory = 100 cycles
        s.slots[slotIndex].access_ts = totalCycles; // update access timestamp
                                                    
      } else if (strcmp(writeHitPolicy,"write-back") == 0) {  // just need to mark block dirty bit to T
        Set &s = cache.sets[index];
        totalCycles++; // access data in cache
        s.slots[slotIndex].access_ts = totalCycles; 
        s.slots[slotIndex].dirty = true;
      }
    }

    else {  // store miss
      storeMisses++;
      if (strcmp(writeMissPolicy, "write-allocate") == 0) {
        totalCycles += (cache.blockSize / 4) * 100; // loading from memory
        Set &s = cache.sets[index]; // store reference to the set

        // getting the index at which the new slot must be placed in set by LRU
        int updatedSlotIndex = -1;
        uint32_t lowestAccessTS = totalCycles;
        uint32_t lowestLoadTS = totalCycles;
        for (int i = 0; i < cache.setSize; i++){
          if (s.slots[i].valid == false){ // set has an empty slot
            updatedSlotIndex = i;
            break;
          } else { // curr slot is full
            if (strcmp(evictPolicy, "lru") == 0){ // evict by LRU
              if (s.slots[i].access_ts < lowestAccessTS){
                lowestAccessTS = s.slots[i].access_ts;
                updatedSlotIndex = i;
              }
            } else if (strcmp(evictPolicy, "fifo") == 0) { // evict by FIFO
              if (s.slots[i].load_ts < lowestLoadTS) {
                lowestLoadTS = s.slots[i].load_ts;
                updatedSlotIndex = i;
              }
            }
          }
        }

        // need to evict what's at this slot
        if (s.slots[updatedSlotIndex].dirty == true){
          totalCycles += 100 * (cache.blockSize/4); // write to memory
          s.slots[updatedSlotIndex].valid = false;
          s.slots[updatedSlotIndex].dirty = false;
        }

        // write following the write-hit policy
        if (strcmp(writeHitPolicy,"write-through") == 0) {
          totalCycles++;       // Write to cache
          totalCycles += 100; // Write to memory

          // update the block in cache
          s.slots[updatedSlotIndex].dirty = false;
          s.slots[updatedSlotIndex].tag = tag;
          s.slots[updatedSlotIndex].valid = true;
          s.slots[updatedSlotIndex].access_ts = totalCycles;
          s.slots[updatedSlotIndex].load_ts = totalCycles; /* for fifo */

        } else if (strcmp(writeHitPolicy,"write-back") == 0) {
          totalCycles++; // Write to cache

          // update the block in the cache
          s.slots[updatedSlotIndex].dirty = true;
          s.slots[updatedSlotIndex].tag = tag;
          s.slots[updatedSlotIndex].valid = true;
          s.slots[updatedSlotIndex].access_ts = totalCycles;
          s.slots[updatedSlotIndex].load_ts = totalCycles; /* for fifo */
        }

      } else if (strcmp(writeMissPolicy, "no-write-allocate") == 0) {
        totalCycles += 100; // Write to memory (no change in cache) //
      }
    }
  }

  else if (operation == 'l') {
    loads++;

    if ((slotIndex != -1) && (cache.sets[index].slots[slotIndex].valid == true)) {  // load hit
      loadHits++;
      totalCycles += 1; // loading from cache for the CPU
      Set &s = cache.sets[index];
      s.slots[slotIndex].access_ts = totalCycles;  // update access timestamp
      
    } else { // load miss
      loadMisses++;
      totalCycles += (cache.blockSize / 4) * 100; // loading block from memory
      totalCycles += 1;                           // loading from cache
      Set &s = cache.sets[index]; // store reference to the set

      // getting the index at which the new slot must be placed in set
      int updatedSlotIndex = -1;
      uint32_t lowestAccessTS = totalCycles;
      uint32_t lowestLoadTS = totalCycles;
      
      for (int i = 0; i < cache.setSize; i++){
        if (s.slots[i].valid == false){
          updatedSlotIndex = i;
          break;
        } else {
          if (strcmp(evictPolicy, "lru") == 0) { // evict by LRU
            if (s.slots[i].access_ts < lowestAccessTS) {
              lowestAccessTS = s.slots[i].access_ts;
              updatedSlotIndex = i;
            }
          } else if (strcmp(evictPolicy, "fifo") == 0) { // evict by FIFO
            if (s.slots[i].load_ts < lowestLoadTS) {
              lowestLoadTS = s.slots[i].load_ts;
              updatedSlotIndex = i;
            }
          }
        }
      }

      // need to evict what's at this slot
      if (s.slots[updatedSlotIndex].dirty == true){
        totalCycles += 100 * (cache.blockSize/4); // write to memory
        s.slots[updatedSlotIndex].valid = false;
        s.slots[updatedSlotIndex].dirty = false;
      }

      // write to cache
      totalCycles++; 
      s.slots[updatedSlotIndex].dirty = false;
      s.slots[updatedSlotIndex].tag = tag;
      s.slots[updatedSlotIndex].valid = true;
      s.slots[updatedSlotIndex].access_ts = totalCycles;
      s.slots[updatedSlotIndex].load_ts = totalCycles; /* for fifo */
    }
  }
}