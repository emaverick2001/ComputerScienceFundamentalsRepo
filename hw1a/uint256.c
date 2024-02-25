#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  // Initialize all words to 0, except for the least significant word.
  for (int i = 1; i < 8; i++){
    result.data[i] = 0;
  }

  result.data[0] = val;
  // TODO: implement for max and msb_set?
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;

  for(int i = 0; i< 8; i++){
    result.data[i]= data[i];
  }

  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;

  char* start = hex[strlen(hex)-1];
  // 64 rightmost hex digits are the least significant?
  // have to make strtol read backwards? how to update endptr (have to reset it now that it reads backwards)
  

  for (int i = 0; i < 8; i++){
    char * endptr = start;
    
    uint32_t val = strtol(start, &endptr, 16);
    result.data[i] = val;

  }
  // iterate through this 8 times for the 8 elements
  // read in hex starting @ least sig digit (furthest right)
  // convert to unsigned long vals
    //stroul -> reads 8 hex char at once (corresponds to 32 bits/ one element in unit256)
    // returns value type u32_t
    //first result stored in most sig, the next towards least sig unti loop ends (keep in dec representation)


  //what does rightmost 64 hex digits refer to? Is it the 64 most sig digits or 64 least sig chars
  char* endptr;
  u_int32_t val = strtoul(hex, &endptr, 16);

  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;
  // TODO: implement
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
