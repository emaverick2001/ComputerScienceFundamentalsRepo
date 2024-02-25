#include "client_util.h"
#include "connection.h"
#include "csapp.h"
#include "message.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  if (!arg_check(server_port, username)){return 1;}

  Connection conn;
  conn.connect(server_hostname, server_port);
  // Connection ERROR
  if (conn.is_open() == false) {
    std::cerr << "Error: Unable to open connection to server";
    return 1;
  }

  /* sending and getting response from rlogin message */
  if (!handle_send(&conn, TAG_RLOGIN, username)) {return 1;}
  
  /* sending and getting response for join message */
  if (!handle_send(&conn, TAG_JOIN, room_name)) {return 1;}

  // loop waiting for messages from server (sent from sender -> server)
  while (conn.is_open()) { 
    Message msg; 
    bool msg_recieved = conn.receive(msg);

    // Error recieving message
    if (msg_recieved == false) {
      std::cerr << "Error recieving message from server.";
      conn.close();
      return 1;
    }

    if (msg.tag.compare(TAG_DELIVERY) == 0) {
      handle_TAG_DELIVERY(msg);
    } else {
      // close connection if err message sent from server
      std::cerr << msg.data;
      conn.close();
      return 1;
    }
  }

  // close connection
  conn.close();
  return 0;
}