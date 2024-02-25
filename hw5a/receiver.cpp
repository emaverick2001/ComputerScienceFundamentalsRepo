#include "client_util.h"
#include "connection.h"
#include "csapp.h"
#include "message.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// reciever
  // rlogin
  // join

// server 
  // rlogin response
  // join response

bool isAlphanumeric(const std::string &str);

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr
        << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  if (server_port <= 1024) {
    std::cerr << "Server port must be greater than 1024" << std::endl;
    return 1;
  }

  Connection conn;

  // TODO: connect to server
  conn.connect(server_hostname, server_port);
  // Connection ERROR
  if (conn.is_open() == false) {
    std::cerr << "Error: Unable to open connection to server";
    return 1;
  }

  /* sending and getting response from rlogin message */

  // sending message
  Message rlogin_msg;
  rlogin_msg.tag = TAG_RLOGIN;
  // rlogin message
  rlogin_msg.data = username;
  // get response from sending message
  bool login_msg_sent = conn.send(rlogin_msg);

  // ERROR sending rlogin message
  if (login_msg_sent == false) {
    std::cerr << "Error sending rlogin message.";
    conn.close();
    return 1;
  }

  // get response to rlogin message from server
  Message rlogin_response_msg;
  bool rlogin_successful = conn.receive(rlogin_response_msg);

  // ERROR recieving login response message
  if (rlogin_successful == false) {
    std::cerr << "Error recieveing response from server for rlogin message.";
    conn.close();
    return 1;
  }
  // Server responded with an ERR tag message
  if (rlogin_response_msg.tag.compare(TAG_ERR) == 0) {
    std::cerr << rlogin_response_msg.data;
    return 1;
  }

  /* sending and getting response for join message */

  // sending message
  Message join_msg;
  join_msg.tag = TAG_JOIN;
  join_msg.data = room_name;
  bool join_msg_sent = conn.send(join_msg);

  // ERROR sending join message
  if (join_msg_sent == false) {
    std::cerr << "Error sending join message to server";
    conn.close();
    return 1;
  }

  // get response to join message from the server
  Message join_msg_response;
  bool join_successful = conn.receive(join_msg_response);

  // ERROR recieving repsonse message to join
  if (join_successful == false) {
    std::cerr << "Error recieving response to join message from server";
    conn.close();
    return 1;
  }

  // Server send back msg with ERR tag
  if (join_msg_response.tag.compare(TAG_ERR) == 0) {
    std::cerr << join_msg_response.data;
    return 1;
  }

  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)

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

      std::string payload = msg.data;
      int first_colon_idx = payload.find(":");
      int second_colon_idx = payload.find(":", first_colon_idx+1);

      std::string room = payload.substr(0, first_colon_idx);
      std::string sender = payload.substr(first_colon_idx + 1, second_colon_idx - first_colon_idx - 1);
      std::string message_text = payload.substr(second_colon_idx + 1, payload.length() - second_colon_idx - 1);
      std::cout << sender << ": " << message_text;
    } else {
      std::cerr << msg.data;
      conn.close();
      return 1;
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
