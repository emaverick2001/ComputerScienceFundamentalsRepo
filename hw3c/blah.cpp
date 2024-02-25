


        // need to evict a slot
        if (updatedSlotIndex = -1){

        }

        // If set is not full, no need to evict
        if (currNumSlotsInSet < cache.setSize) {
          // store new block that was added to cache
          // TODO save index of slot in set into map
          s.slotsMap[tag] = currNumSlotsInSet;
          // update new slot info
          s.slots[currNumSlotsInSet].valid = true;
          s.slots[currNumSlotsInSet].tag = tag;
          s.slots[currNumSlotsInSet].access_ts = totalCycles;

          // write to memory
          if (strcmp(writeHitPolicy,"write-through")) {
            totalCycles++;       // Write to cache
            totalCycles += 100;  // update data and cycles
          } else if (strcmp(writeHitPolicy,"write-back")) {
            totalCycles++;  // Write to cache
            // mark block dirty bit to T
            s.slots[currNumSlotsInSet].dirty = true;
            s.slots[currNumSlotsInSet].access_ts = totalCycles;
          }
        } else {  // if set is full, need to evict
          // if evict policy is least recently used
          if (strcmp(evictPolicy,"lru")) {
            // find index of least recently used block in set
            uint32_t lowestAccessTS = totalCycles;
            uint32_t indexLowestAccessTS = -1;
            unsigned int setSize = cache.sets[index].slots.size();

            for (unsigned int i = 0; i < setSize; i++) {
              Slot currSlot = cache.sets[index].slots[i];
              if (currSlot.access_ts < lowestAccessTS) {
                lowestAccessTS = currSlot.access_ts;
                indexLowestAccessTS = i;
              }

              totalCycles += (cache.blockSize /
                              4);  // because we accessed a block in cache
            }

            // check if dirty bit
            if (cache.sets[index].slots[indexLowestAccessTS].dirty == true) {
              totalCycles =
                  totalCycles + 100;  // write the current slot to memory
            }
            // overwrite lru block
            // if curr write hit policy is write through
            if (strcmp(writeHitPolicy,"write-through")) {
              totalCycles++;       // Write to cache
              totalCycles += 100;  // update data and cycles

              // Update the slot
              cache.sets[index].slots[indexLowestAccessTS].dirty = false;
              cache.sets[index].slots[indexLowestAccessTS].valid = true;

              cache.sets[index].slots[indexLowestAccessTS].tag = tag;

              cache.sets[index].slots[indexLowestAccessTS].access_ts =
                  totalCycles;
            }

            // if curr write hit policy is write back
            else if (strcmp(writeHitPolicy,"write-back")) {
              totalCycles++;  // Write to cache

              // Update a new slot
              cache.sets[index].slots[indexLowestAccessTS].dirty = true;
              cache.sets[index].slots[indexLowestAccessTS].valid = true;
              cache.sets[index].slots[indexLowestAccessTS].tag = tag;

              cache.sets[index].slots[indexLowestAccessTS].access_ts =
                  totalCycles;
            }
          }
        }













      int currNumSlotsInSet = cache.sets[index].slots.size();
      // load from mem = 100 cycles
      totalCycles += (cache.blockSize / 4) * 100;
      // if set is not full, no need to evict before copying it into the cache
      if (currNumSlotsInSet < cache.setSize) {
        totalCycles++;

        Set &s = cache.sets[index];
        s.slots[index].dirty = false;
        s.slots[index].valid = true;
        s.slots[index].tag = tag;
        s.slots[index].access_ts = totalCycles;

      } else {  // need to evict
        if (strcmp(evictPolicy, "lru") == 0) {
          // find index of least recently used block in set
          uint32_t lowestAccessTS = totalCycles;
          uint32_t indexLowestAccessTS = -1;
          unsigned int setSize = cache.sets[index].slots.size();

          for (unsigned int i = 0; i < setSize; i++) {
            Slot currSlot = cache.sets[index].slots[i];
            if (currSlot.access_ts < lowestAccessTS) {
              lowestAccessTS = currSlot.access_ts;
              indexLowestAccessTS = i;
            }

            totalCycles +=
                (cache.blockSize / 4);  // because we accessed a block in cache
          }

          // check if dirty bit
          if (cache.sets[index].slots[indexLowestAccessTS].dirty == true) {
            totalCycles =
                totalCycles + 100;  // write the current slot to memory
          }

          // overwrite lru block
          totalCycles = totalCycles + 1;  // Write to cache
          // Update the slot
          cache.sets[index].slots[indexLowestAccessTS].dirty = false;
          cache.sets[index].slots[indexLowestAccessTS].valid = true;

          cache.sets[index].slots[indexLowestAccessTS].tag = tag;

          cache.sets[index].slots[indexLowestAccessTS].access_ts = totalCycles;
        }
      }
      }

      // bool Cache::checkIfHit(uint32_t address) {
      //     // if valid bit is true && tag matches, then it is a hit
      //     u_int32_t tag = 0;
      //     u_int32_t index = 0;
      //     getTagAndIndex(address,tag,index);
      //     int numSlotsInSet = this->sets[index].slots.size();
      //     // cout<<index<<endl;
      //     // search through the set to see if there is a hit
      //     for (int i = 0; i < numSlotsInSet; i++) {
      //         if (sets[index].slots[i].valid == true &&
      //         sets[index].slots[i].tag == tag) {
      //             return true;
      //         }
      //     }
      //     // cout<<"block not in cache"<<endl;
      //     return false;
      // }

      // int Cache::getSlotIndex(std::vector<Slot> set, uint32_t tag) {
      //     unsigned int setSize = set.size();
      //     // cout << "setsize: " << setSize << endl;
      //     int slotIndex = -1;
      //     // cout << "REACHED HERE 1 tag given:" << tag << endl;

      //     for (unsigned int i = 0; i < setSize; i++) {
      //         Slot currSlot = set[i];
      //         // cout << "REACHED HERE 2 (for loop) currSlotTag:" <<
      //         currSlot.tag << endl; if (currSlot.tag == tag) { slotIndex = i;
      //         }
      //   }

      //   return slotIndex;
      // }