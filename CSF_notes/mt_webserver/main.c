/*
 * Very basic webserver
 * Only supports GET requests, only serves files, content types
 * are hard-coded
 *
 * Multithreaded version using pthreads
 *
 * main function
 */

#include <stdio.h>
#include <pthread.h>
#include "csapp.h"
#include "webserver.h"

/*
 * Data structure representing a client connection.
 */
struct ConnInfo {
	int clientfd;
	const char *webroot;
};

void *worker(void *arg) {
	struct ConnInfo *info = arg;

	/*
	 * set thread as detached, so its resources are automatically
	 * reclaimed when it finishes
	 */
	pthread_detach(pthread_self());

	/* handle client request */
	server_chat_with_client(info->clientfd, info->webroot);
	close(info->clientfd);
	free(info);

	return NULL;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		fatal("Usage: webserver <port> <webroot>");
	}

	const char *port = argv[1];
	const char *webroot = argv[2];

	int serverfd = open_listenfd((char*) port);
	if (serverfd < 0) {
		fatal("Couldn't open server socket");
	}

	while (1) {
		int clientfd = Accept(serverfd, NULL, NULL);
		if (clientfd < 0) {
			fatal("Error accepting client connection");
		}

		/* create ConnInfo object */
		struct ConnInfo *info = malloc(sizeof(struct ConnInfo));
		info->clientfd = clientfd;
		info->webroot = webroot;

		/* start new thread to handle client connection */
		pthread_t thr_id;
		if (pthread_create(&thr_id, NULL, worker, info) != 0) {
			fatal("pthread_create failed");
		}
	}
}
