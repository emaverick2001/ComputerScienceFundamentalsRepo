#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main() {
    // Input hexadecimal string
    const char* hexString = "0xAB0478C9EF";

    // Use strtoull to convert the hexadecimal string to uint64_t
    char* endPtr;
    uint32_t result = (uint32_t)strtoull(hexString, &endPtr, 16);
    uint32_t result2 = (uint32_t)strtoull(endPtr, &endPtr, 16);

    // Check for conversion errors
    if (*endPtr != '\0') {
        printf("Conversion error: Not a valid hexadecimal number\n");
        return 1;
    }

    // Print the result
    printf("Hexadecimal String: %s\n", hexString);
    printf("Equivalent uint64_t: %" PRIu64 "\n", result);

    return 0;
}