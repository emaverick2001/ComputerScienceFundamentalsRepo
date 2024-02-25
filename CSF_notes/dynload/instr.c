#include <stdlib.h>
#include <dlfcn.h>

int (*real_puts)(const char *s);

int puts(const char *s) {
  if (!real_puts) {
    void *handle = dlopen("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
    if (!handle) { exit(1); }
    *(void **) (&real_puts) = dlsym(handle, "puts");
    if (!real_puts) { exit(1); }
  }

  real_puts("This is the interposed version of puts!");
  return real_puts(s);
}
