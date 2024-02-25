#include <stdio.h>

int main(void) {
  // Declare a pointer to a function returning int and taking
  // a single const char * parameter.
  int (*ptr)(const char *);

  // Make the function pointer point to a compatible function.
  ptr = puts;

  // Use the pointer to call the function it points to.
  ptr("Hello world");

  return 0;
}
