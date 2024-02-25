/*
 * Very basic webserver
 * Only supports GET requests, only serves files, content types
 * are hard-coded
 *
 * Implementation of functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "csapp.h"
#include "webserver.h"

#define MAX_LINE_LEN 4096
#define MAX_NUM_HEADERS 128

/*
 * Called to indicate a fatal error.
 * Prints an error message to stderr and exits the program.
 */
void fatal(const char *msg) {
	fprintf(stderr, "error: %s\n", msg);
	exit(1);
}

/*
 * Allocate memory, calling fatal if the allocation fails.
 */
void *xmalloc(size_t nbytes) {
	void *p = malloc(nbytes);
	if (!p) {
		fatal("malloc failed");
	}
	return p;
}

/*
 * Duplicate a string, calling fatal if the allocation fails.
 */
char *xstrdup(const char *s) {
	size_t len = strlen(s);
	char *dup = xmalloc(len + 1);
	strcpy(dup, s);
	return dup;
}

/*
 * Convenience function to read a line and trim trailing
 * CR or CRLF.
 */
ssize_t readline(rio_t *in, char *usrbuf, size_t maxlen) {
	ssize_t len = rio_readlineb(in, usrbuf, maxlen);
	if (len > 0 && usrbuf[len-1] == '\n') {
		/* trim trailing LF (newline) */
		usrbuf[len-1] = '\0';
		len--;
	}
	if (len > 0 && usrbuf[len-1] == '\r') {
		/* trim trailing CR */
		usrbuf[len-1] = '\0';
		len--;
	}
	return len;
}

/*
 * Write a string to a file descriptor.
 */
void writestr(int outfd, const char *s) {
	size_t len = strlen(s);
	rio_writen(outfd, (char*)s, len);
}

/*
 * Write a long int to a file descriptor (in decimal format.)
 */
void writelong(int outfd, long val) {
	char buf[40];
	snprintf(buf, 40, "%ld", val);
	writestr(outfd, buf);
}

/*
 * Safely concatenate two strings.
 * Returns a dynamically allocated result string.
 */
char *concat(const char *s1, const char *s2) {
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	char *result = xmalloc(len1 + len2 + 1);
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2);
	result[len1+len2] = '\0';
	return result;
}

/*
 * Copy as many bytes as possible from fromfd to tofd.
 */
void copyto(int fromfd, int tofd) {
	char *buf = xmalloc(4096);
	int done = 0;
	while (!done) {
		ssize_t n;
		n = rio_readn(fromfd, buf, 4096);
		if (n <= 0) {
			done = 1;
		} else {
			n = rio_writen(tofd, buf, n);
			if (n < 0) {
				done = 1;
			}
		}
	}
	free(buf);
}

/*
 * Create a struct Header object with specified name and content values.
 */
struct Header *header_create(const char *name, const char *content) {
	struct Header *hdr = xmalloc(sizeof(struct Header));
	hdr->name = xstrdup(name);
	hdr->content = xstrdup(content);
	return hdr;
}

/*
 * Destroy a struct Header object by de-allocating its memory.
 */
void header_destroy(struct Header *hdr) {
	if (hdr) {
		free(hdr->name);
		free(hdr->content);
		free(hdr);
	}
}

/*
 * Read an HTTP request from specified rio_t input channel,
 * returning a pointer to a struct Message object representing
 * the request.  Returns NULL if a valid request couldn't
 * be read.
 */
struct Message *message_read_request(rio_t *in) {
	struct Message *result = NULL, *msg = NULL;
	char *linebuf = xmalloc(MAX_LINE_LEN);
	ssize_t len;

	/* read and parse first line of request */
	len = readline(in, linebuf, MAX_LINE_LEN);
	if (len < 0) { goto read_done; }
	char *savep, *method, *resource, *proto;
	method = strtok_r(linebuf, " ", &savep);
	if (!method) { goto read_done; }
	resource = strtok_r(NULL, " ", &savep);
	if (!resource) { goto read_done; }
	proto = strtok_r(NULL, " ", &savep);
	if (!proto || strcmp(proto, "HTTP/1.1") != 0) { goto read_done; }

	/* create the Message object */
	msg = xmalloc(sizeof(struct Message));
	memset(msg, 0, sizeof(struct Message));
	msg->method = xstrdup(method);
	msg->resource = xstrdup(resource);
	msg->headers = xmalloc(MAX_NUM_HEADERS * sizeof(struct Header *));

	/* read headers */
	int num_headers = 0;
	int done_with_headers = 0;
	while (!done_with_headers) {
		len = readline(in, linebuf, MAX_LINE_LEN);
		if (len < 0) { goto read_done; }
		if (len == 0) {
			/* read EOF? */
			done_with_headers = 1;
		} else if (strcmp(linebuf, "") == 0) {
			/* read a blank line */
			done_with_headers = 1;
		} else if (num_headers < MAX_NUM_HEADERS) {
			/* try to read a header */
			char *p = strchr(linebuf, ':');
			if (p) {
				*p = '\0'; /* NUL-terminate name */
				p++; /* skip to character following colon */
				while (*p && isspace(*p)) {
					p++; /* skip space preceding content */
				}
				struct Header *hdr = xmalloc(sizeof(struct Header));
				hdr->name = xstrdup(linebuf);
				hdr->content = xstrdup(p);
				msg->headers[num_headers] = hdr;
				num_headers++;
			}
		}
	}

	/* read message body? */

	/* successfully read the request */
	result = msg;
	msg = NULL;

read_done:
	message_destroy(msg);
	free(linebuf);
	return result;
}

/*
 * Destroy a struct Message object by de-allocating its memory.
 */
void message_destroy(struct Message *msg) {
	if (msg) {
		free(msg->method);
		free(msg->resource);
		if (msg->headers) {
			for (int i = 0; i < msg->num_headers; i++) {
				header_destroy(msg->headers[i]);
			}
		}
		free(msg->headers);
		free(msg);
	}
}

/*
 * Chat with a client by reading a request, processing it, and
 * generating an appropriate response.
 */
void server_chat_with_client(int clientfd, const char *webroot) {
	struct Message *req = NULL;
	rio_t in;

	rio_readinitb(&in, clientfd);

	req = message_read_request(&in);
	printf("got request for resource %s\n", req->resource);
	if (req) {
		server_generate_response(clientfd, req, webroot);
		message_destroy(req);
	}
}

void server_generate_response(int clientfd, struct Message *req, const char *webroot) {
	char *filename = NULL;

	/* only GET requests are supported */
	if (strcmp(req->method, "GET") != 0) {
		server_generate_text_response(clientfd, "403", "Forbidden",
			"only GET requests are allowed");
		goto response_done;
	}

	/*
	 * for now, assume all requests are for files
	 * in the webroot
	 */
	filename = concat(webroot, req->resource);
	struct stat s;
	if (stat(filename, &s) < 0) {
		server_generate_text_response(clientfd, "404", "Not Found",
			"requested resource does not exist");
		goto response_done;
	}

	/* write response line */
	writestr(clientfd, "HTTP/1.1 200 OK\r\n");

	/* write Content-Type and Content-Length headers */
	const char *content_type = server_determine_content_type(filename);
	if (content_type) {
		writestr(clientfd, "Content-Type: ");
		writestr(clientfd, content_type);
		writestr(clientfd, "\r\n");
	}
	writestr(clientfd, "Content-Length: ");
	writelong(clientfd, (long) s.st_size);
	writestr(clientfd, "\r\n");

	writestr(clientfd, "\r\n");

	/* send file data */
	int fd = Open(filename, O_RDONLY, 0);
	copyto(fd, clientfd);
	close(fd);

response_done:
	free(filename);
}

void server_generate_text_response(int clientfd, const char *response_code,
	const char *reason, const char *msg) {
	writestr(clientfd, "HTTP/1.1 ");
	writestr(clientfd, response_code);
	writestr(clientfd, " ");
	writestr(clientfd, reason);
	writestr(clientfd, "\r\n");
	/* could generate headers... */
	writestr(clientfd, "\r\n");
	writestr(clientfd, msg);
}

/*
 * struct Header is abused here to represent the entries
 * in a lookup table to determine Content-Type based on
 * file extension
 */
static struct Header s_content_type_lookup[] = {
	{ ".jpg", "image/jpeg" },
	{ ".JPG", "image/jpeg" },
	{ ".png", "image/png" },
	{ ".PNG", "image/png" },
	{ ".html", "text/html; charset=utf-8" },
	{ ".htm", "text/html; charset=utf-8" },
	{ NULL },
};

/*
 * Determine appropriate Content-Type value for specified filename;
 * returns NULL if Content-Type cannot be determined.
 */
const char *server_determine_content_type(const char *filename) {
	const char *fext = strrchr(filename, '.');
	if (!fext) {
		return NULL;
	}
	for (int i = 0; s_content_type_lookup[i].name != NULL ; i++) {
		if (strcmp(s_content_type_lookup[i].name, fext) == 0) {
			return s_content_type_lookup[i].content;
		}
	}
	return NULL;
}
