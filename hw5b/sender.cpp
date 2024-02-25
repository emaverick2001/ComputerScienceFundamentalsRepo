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

  // Error handling for username + room name
  if (!arg_check(server_port, username)){return 1;}
  
  // Connect to server
  Connection conn;
  conn.connect(server_hostname, server_port);

  // Connection ERROR
  if (conn.is_open() == false) {
    std::cerr << "Error: Unable to open connection to server" << std::endl;
    conn.close();
    return 1;
  }

  // send slogin message
  if (!handle_send(&conn, TAG_SLOGIN, username)) {
    return 1;
  }
  
  // loop reading commands from user, sending messages to server as appropriate
  while (conn.is_open()) {

    Message msg;
    if (!get_msg(&msg)) {continue;}
    
    // Sends one of the messages handled above
    std::string response_tag;
    if (!handle_response_no_quit(msg, &conn, &response_tag)) {return 1;}

    if (msg.tag == TAG_QUIT && response_tag == TAG_OK) {
      conn.close();
      break;
    }
  }

  // close connection
  conn.close();
  return 0;
}