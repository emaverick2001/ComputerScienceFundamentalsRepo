#include "Set.h"

using std::cout;
using std::endl;

Set::Set(int setSize, int blockSize) : setSize(setSize), blockSize(blockSize) {
    for (int i = 0; i < setSize; i++) {
        Slot s = Slot(false,false);
        slots.push_back(s);
    }
}