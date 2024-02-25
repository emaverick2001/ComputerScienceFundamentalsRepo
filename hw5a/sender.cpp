#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

bool isAlphanumeric(const std::string &str);

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // TODO error handling for username + room name
  if (!isAlphanumeric(username)) {
    std::cerr << "Invalid characters in username" << std::endl;
    return 1;
  }

  if (server_port <= 1024) {
    std::cerr << "Server port must be greater than 1024" << std::endl;
    return 1;
  }

  // TODO: connect to server
  Connection conn;
  conn.connect(server_hostname, server_port);

  // Connection ERROR
  if (conn.is_open() == false) {
    std::cerr << "Error: Unable to open connection to server" << std::endl;
    conn.close();
    return 1;
  }

  // TODO: send slogin message
  Message slogin_msg;
  slogin_msg.tag = TAG_SLOGIN;
  slogin_msg.data = username;

  bool slogin_msg_sent = conn.send(slogin_msg);
  // ERROR sending slogin message
  if (slogin_msg_sent == false) {
    std::cerr << "Error sending slogin message" << std::endl;
    conn.close();
    return 1;
  }

  // get response to slogin message from server
  Message slogin_response;
  bool slogin_response_recieved = conn.receive(slogin_response);

  // ERROR recieving repsonse from server to slogin message
  if (slogin_response_recieved == false) {
    std::cerr << "Error recieving response from server to slogin message" << std::endl;
    conn.close();
    return 1;
  }

  // Server response to slogin was ERR tag
  if (slogin_response.tag.compare(TAG_ERR) == 0) {
    std::cerr << slogin_response.data;
    conn.close();
    return 1;
  }

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  while (conn.is_open()) {
    // read from std::in
    std::string sender_request;
    std::getline(std::cin, sender_request);
    std::stringstream ss(sender_request);
    std::string command;
    ss >> command;

    Message msg;

    // handling command messages
    if (command == "/join") {
      msg.tag = TAG_JOIN;
      std::getline(ss, msg.data);
      msg.data = trim(msg.data);
    } else if (command == "/leave") {
      msg.tag = TAG_LEAVE;
      msg.data = "leave";
    } else if (command == "/quit") {
      msg.tag = TAG_QUIT;
      msg.data = "quit";
    } else if (command[0] == '/') {
      std::cerr << "Error entering command. Acceptable commands are /join,"
                   "/leave, /quit" << std::endl;
      continue;
    } else {
      msg.tag = TAG_SENDALL;
      msg.data = trim(ss.str());
    }

    // Sends one of the messages handled above
    bool msg_sent = conn.send(msg);
    if (msg_sent == false) {
      std::cerr << "Error sending message to server" << std::endl;
      conn.close();
      return 1;
    }

    Message response;
    bool msg_response_received = conn.receive(response);
    if (msg_response_received == false) {
      std::cerr << "Error recieving message from server" << std::endl;
      conn.close();
      return 1;
    }

    if (response.tag.compare(TAG_ERR) == 0) {
      std::cerr << response.data;
    }

    if (msg.tag == TAG_QUIT && response.tag == TAG_OK) {
      conn.close();
      break;
    }
  }

  // close connection
  conn.close();
  return 0;
}

bool isAlphanumeric(const std::string &str) {
  for (char ch : str) {
      if (!std::isalnum(ch)) {
          return false;
      }
  }
  return true;
}