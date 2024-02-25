#include <stdlib.h>
#include <string.h>
#include "csapp.h"

void fatal(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

// Return 0 if client sends "quit" message, 1 otherwise.
int chat_with_client(int client_fd) {
  rio_t rio;
  int sum = 0, val;

  // Initialize the rio_t object for buffered reading
  rio_readinitb(&rio, client_fd);

  // Read line from client
  char buf[1024];
  ssize_t rc = rio_readlineb(&rio, buf, sizeof(buf));
  if (rc < 0) {
    return 1; // error reading data from client
  }

  if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "quit\r\n") == 0) {
    return 0;
  } else {
    FILE *in = fmemopen(buf, (size_t) rc, "r");
    while (fscanf(in, "%d", &val) == 1) { sum += val; }
    fclose(in);

    // Send message back to client
    snprintf(buf, sizeof(buf), "Sum is %d\n", sum);
    rio_writen(client_fd, buf, strlen(buf));

    return 1;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fatal("Usage: ./arithserver <port>");
  }

  int server_fd = open_listenfd(argv[1]);
  if (server_fd < 0) {
    fatal("Couldn't open server socket\n");
  }

  int keep_going = 1;

  while (keep_going) {
    int client_fd = Accept(server_fd, NULL, NULL);
    if (client_fd > 0) {
      keep_going = chat_with_client(client_fd);
      // close the connection
      close(client_fd);
    }
  }

  // close server socket
  close(server_fd);

  return 0;
}
