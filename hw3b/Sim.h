#ifndef SIM_H
#define SIM_H
#include <stdint.h>
#include "Cache.h"


class Sim {
    public:
        // Constructor
        Sim(Cache cache);

        void run(char operation, u_int32_t address, char* writeMissPolicy, char* writeHitPolicy, char* evictPolicy); // analyzes a single trace
        
        // Prints summary information 
        void simSummary();
        
    private:
        Cache cache;
        int loads;
        int stores;
        int loadHits;
        int loadMisses;
        int storeHits;
        int storeMisses;
        int totalCycles;
};

#endif // SIM_H