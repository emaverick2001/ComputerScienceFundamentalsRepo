/*
 * This program was shamelessly adapted/plagiarized from:
 *   https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/tcpserver.c
 *
 * Compile using:
 *   gcc -Wall -o server server.c
 *
 * Run the server using (port number is arbitrary):
 *   ./server 30000
 *
 * Testing (using port number that server is listening on):
 *   telnet localhost 30000
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void fatal(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

int create_server_socket(int port) {
  struct sockaddr_in serveraddr = {0};
  int ssock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock_fd < 0) 
    fatal("socket failed");

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);
  if (bind(ssock_fd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    fatal("bind failed");

  if (listen(ssock_fd, 5) < 0)
    fatal("listen failed");

  return ssock_fd;
}

int accept_connection(int ssock_fd, struct sockaddr_in *clientaddr) {
  unsigned clientlen = sizeof(clientaddr);
  int client_fd = accept(ssock_fd, (struct sockaddr *) clientaddr, &clientlen);
  if (client_fd < 0) 
    fatal("accept failed");
  return client_fd;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fatal("Usage: ./server <port>");
  }

  char buf[256];
  int port = atoi(argv[1]);
  int ssock_fd = create_server_socket(port);

  while (1) {
    struct sockaddr_in clientaddr;
    int clientfd = accept_connection(ssock_fd, &clientaddr);
    ssize_t rc = read(clientfd, buf, sizeof(buf));
    if (rc > 0) {
      write(clientfd, buf, rc);
    }
    close(clientfd);
  }
}
