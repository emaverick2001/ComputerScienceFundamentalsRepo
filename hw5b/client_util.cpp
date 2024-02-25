#include "client_util.h"
#include "connection.h"
#include "message.h"
#include <iostream>
#include <sstream>
#include <string>

// string trim functions shamelessly stolen from
// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

bool isAlphanumeric(const std::string &str) {
  for (char ch : str) {
    if (!std::isalnum(ch)) {
      return false;
    }
  }
  return true;
}

bool handle_send(Connection *conn, std::string TAG, std::string msg_data) {
  // sending message
  Message msg;
  msg.tag = TAG;
  msg.data = msg_data;
  // get response from sending message
  bool msg_sent = conn->send(msg);

  // ERROR sending login message
  if (msg_sent == false) {
    std::cerr << "Error sending rlogin message.";
    conn->close();
    return false;
  }

  if (!handle_response(conn)) {
    return false;
  }
  return true;
}

bool handle_response(Connection *conn) {
  Message response_msg;
  bool response = conn->receive(response_msg);

  // ERROR recieving response message
  if (response == false) {
    std::cerr << "Error recieveing response from server.";
    conn->close();
    return false;
  }
  // Server responded with an ERR tag message
  if (response_msg.tag.compare(TAG_ERR) == 0) {
    std::cerr << response_msg.data;
    return false;
  }

  return true;
}

void handle_TAG_DELIVERY(Message msg) {
  std::string payload = msg.data;
  int first_colon_idx = payload.find(":");
  int second_colon_idx = payload.find(":", first_colon_idx + 1);

  std::string room = payload.substr(0, first_colon_idx);
  std::string sender = payload.substr(first_colon_idx + 1,
                                      second_colon_idx - first_colon_idx - 1);
  std::string message_text = payload.substr(
      second_colon_idx + 1, payload.length() - second_colon_idx - 1);
  // print message on reciever/ client side
  std::cout << sender << ": " << message_text;
}

bool arg_check(int server_port, std::string username) {
  if (server_port <= 1024) {
    std::cerr << "Server port must be greater than 1024" << std::endl;
    return false;
  }

  // Error handling for username + room name
  if (!isAlphanumeric(username)) {
    std::cerr << "Invalid characters in username" << std::endl;
    return false;
  }

  return true;
}


bool handle_response_no_quit(Message msg, Connection *conn, std::string *response_tag) {
  // Sends one of the messages handled above
  bool msg_sent = conn->send(msg);
  if (msg_sent == false) {
    std::cerr << "Error sending message to server" << std::endl;
    conn->close();
    return false;
  }

  Message response;
  bool msg_response_received = conn->receive(response);
  if (msg_response_received == false) {
    std::cerr << "Error recieving message from server" << std::endl;
    conn->close();
    return false;
  }

  // if server sends back error print to cerr
  if (response.tag.compare(TAG_ERR) == 0) {
    std::cerr << response.data;
  }

  *response_tag = response.tag;

  return true;
}

// return: true - got message successfully; false - invalid command
bool get_msg(Message *msg) {
  std::string sender_request;
  std::getline(std::cin, sender_request);
  std::stringstream ss(sender_request);
  std::string command;
  ss >> command;

  if (command == "/join") {
    msg->tag = TAG_JOIN;
    std::getline(ss, msg->data);
    msg->data = trim(msg->data);
  } else if (command == "/leave") {
    msg->tag = TAG_LEAVE;
    msg->data = "leave";
  } else if (command == "/quit") {
    msg->tag = TAG_QUIT;
    msg->data = "quit";
  } else if (command[0] == '/') {
    std::cerr << "Error entering command. Acceptable commands are /join,"
                 "/leave, /quit"
              << std::endl;
    return false;
  } else {
    msg->tag = TAG_SENDALL;
    msg->data = trim(ss.str());
  }

  return true;
}