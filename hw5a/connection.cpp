#include "connection.h"
#include "client_util.h"
#include "csapp.h"
#include "message.h"
#include <cassert>
#include <cctype>
#include <linux/limits.h>
#include <sstream>

// rio type
// typedef struct {
//     int rio_fd;                /* Descriptor for this internal buf */
//     int rio_cnt;               /* Unread bytes in internal buf */
//     char *rio_bufptr;          /* Next unread byte in internal buf */
//     char rio_buf[RIO_BUFSIZE]; /* Internal buffer */
// } rio_t;

Connection::Connection() : m_fd(-1), m_last_result(SUCCESS) {}

Connection::Connection(int fd) : m_fd(fd), m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // call open_clientfd to connect to the server
  m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  // call rio_readinitb to initialize the rio_t object, successful if m_fd > 0 
  if (m_fd > -1) {
    rio_readinitb(&m_fdbuf, m_fd);
    m_last_result = SUCCESS;
  } else {
    m_last_result = EOF_OR_ERROR;
  }
}

Connection::~Connection() {
  // Close the socket if it is open
  if (is_open() == true) {
    close();
  }
}

bool Connection::is_open() const {
  // Return true if the connection is open
  if (m_fd > -1) {
    return true;
  } else {
    return false;
  }
}

void Connection::close() {
  // Close the connection if it is open
  if (is_open()) {
    ::close(m_fd);
    m_fd = -1;
  }
}

bool Connection::send(const Message &msg) {
  // msg format will be tag:payload.
  // If tag is invalid, then invalid msg
  std::string valid_tags[11] = {
      TAG_ERR,     TAG_OK,       TAG_SLOGIN, TAG_RLOGIN,   TAG_JOIN, TAG_LEAVE,
      TAG_SENDALL, TAG_SENDUSER, TAG_QUIT,   TAG_DELIVERY, TAG_EMPTY};
  bool is_tag_valid = false;
  for (int i = 0; i < 11; i++) {
    if (msg.tag.compare(valid_tags[i]) == 0) {
      is_tag_valid = true;
      break;
    }
  }
  // if invalid tag
  if (is_tag_valid == false) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // create the message in format tag:payload
  std::string msg_formatted = msg.tag + ":" + msg.data + "\n";
  const char *usr_buf = msg_formatted.c_str();

  // sends message from client to server
  int num_bytes_written = rio_writen(m_fd, usr_buf, msg_formatted.length());

  // if num_bytes_written <=0 or < length of message- EOF or Error
  if (num_bytes_written != (int)msg_formatted.length()) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  // else successfully wrote message
  m_last_result = SUCCESS;
  return true;
}

 
bool Connection::receive(Message &msg) {
  char user_buf[MAX_INPUT];
  // read message from server
  int num_bytes_read = rio_readlineb(&m_fdbuf, user_buf, MAX_INPUT);

  // did not read successfully if num_bytes read <= 0 - hit EOF error
  if (num_bytes_read <= 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  // Parse user_buf and store tag in tag and payload in data of msg
  std::string message = user_buf; // string of recieved info

  // did not find colon
  int colon_idx = message.find(":");
  if (colon_idx == (int) std::string::npos) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // checking if valid tag
  msg.tag = message.substr(0, colon_idx);
  std::string valid_tags[11] = {
      TAG_ERR,     TAG_OK,       TAG_SLOGIN, TAG_RLOGIN,   TAG_JOIN, TAG_LEAVE,
      TAG_SENDALL, TAG_SENDUSER, TAG_QUIT,   TAG_DELIVERY, TAG_EMPTY};
  bool is_tag_valid = false;
  for (int i = 0; i < 11; i++) {
    if (msg.tag.compare(valid_tags[i]) == 0) {
      is_tag_valid = true;
      break;
    }
  }
  // invalid tag
  if (is_tag_valid == false) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // else successful.
  msg.data = message.substr(colon_idx + 1, num_bytes_read - 1 - colon_idx);
  m_last_result = SUCCESS;
  return true;
}
