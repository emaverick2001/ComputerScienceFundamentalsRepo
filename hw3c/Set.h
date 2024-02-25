#ifndef SET_H
#define SET_H

#include <iostream>
#include <vector>
#include <map>
using std::vector;
using std::map;

struct Slot {
    bool dirty;
    bool valid;
    u_int32_t tag;
    u_int32_t load_ts;
    u_int32_t access_ts;
    Slot(bool dirty, bool valid) : dirty(dirty), valid(valid), tag(0), load_ts(0), access_ts(0) {} 
};

class Set {   
    public:
    // Constructor
    Set(int setSize, int blockSize);
    
    map <int, int> slotsMap; 
    vector<Slot> slots;

    private:
    int setSize; //use to determine index: log2 of this
    int blockSize; //use to determine offset: log 2 of this
};



#endif