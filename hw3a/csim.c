#include <stdio.h>

int main(int argc, char *argv[]) {
    // argc is the argument count, including the program name itself
    // argv is an array of strings containing the arguments
    if (argc != 8) {
        printf("Usage: %s <cache size> <block size> <set associative> <write policy> <replacement policy> <trace file>\n", argv[0]);
        return 1;
    }
    
    // argv[0] = execute command
    int numSets = atoi(argv[1]);
    int numBlocks = atoi(argv[2]);
    int numBytes = atoi(argv[3]);
    if (numBytes < 4){
        return 1;
    }
    char *param1 = argv[4];
    char *param2 = argv[5];
    char *param3 = argv[6];
    FILE *in = fopen(argv[7], "r");

    if (in == NULL) {
        perror("Error opening trace file");
        return 1;
    }

    
    int totalLoads = 0;
    int totalStores = 0;
    int loadHits = 0;
    int loadMisses = 0;
    int storeHits = 0;
    int storeMisses = 0;
    int totalCycles = 0;

    //error handling


    fclose(in);

    // PRINTING CACHE INFORMATION

    return 0;
}