/*
 * Very basic webserver
 * Only supports GET requests, only serves files, content types
 * are hard-coded
 *
 * main header file
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stddef.h>
#include <sys/types.h>

/* data type for message headers */
struct Header {
	char *name;
	char *content;
};

/* Message data type, represents a request from a client */
struct Message {
	int num_headers;          /* number of headers */
	struct Header **headers;  /* array of headers */
	char *method;             /* the method */
	char *resource;           /* the resource requested */
};

/* general helper functions */
void fatal(const char *msg);
void *xmalloc(size_t nbytes);
char *xstrdup(const char *s);
ssize_t readline(rio_t *in, char *usrbuf, size_t maxlen);
void writestr(int outfd, const char *s);
void writelong(int outfd, long val);
char *concat(const char *s1, const char *s2);
void copyto(int fromfd, int tofd);

/* Header functions */
struct Header *header_create(const char *name, const char *content);
void header_destroy(struct Header *hdr);

/* Message functions */
struct Message *message_read_request(rio_t *in);
void message_destroy(struct Message *msg);

/* server functions */
void server_chat_with_client(int clientfd, const char *webroot);
void server_generate_response(int clientfd, struct Message *req, const char *webroot);
void server_generate_text_response(int clientfd, const char *response_code,
	const char *reason, const char *msg);
const char *server_determine_content_type(const char *filename);

#endif /* WEBSERVER_H */
