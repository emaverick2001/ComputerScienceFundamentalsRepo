#include <stdlib.h>
#include <string.h>
#include "csapp.h"

void fatal(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fatal("Usage: ./arithclient <hostname> <port> <message>");
  }

  int fd = open_clientfd(argv[1], argv[2]);
  if (fd < 0) {
    fatal("Couldn't connect to server");
  }

  // send message to server
  rio_writen(fd, argv[3], strlen(argv[3]));
  rio_writen(fd, "\n", 1);

  // set up rio_t (robust I/O handle) to read data from server
  rio_t rio;
  rio_readinitb(&rio, fd);

  // read response from server
  char buf[1000];
  ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));

  // print response
  if (n > 0) {
    printf("Received from server: %s", buf);
  }

  close(fd);
  return 0;
}
