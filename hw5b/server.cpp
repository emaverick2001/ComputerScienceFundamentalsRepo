#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"
#include "client_util.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

struct ConnectionInfo
{
  // Used to pass Connection object
  Connection *curr_connection;
  // need server instance to access find_or_create_room
  Server *server;
  ~ConnectionInfo()
  {
    delete curr_connection;
  }
};


////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

void chat_with_receiver(ConnectionInfo *connInfo,User *user) {
  Room *room = nullptr;
  Message client_msg;
  Message server_response_msg;

  // create a User object in each client thread to track the pending messages
  if (!connInfo->curr_connection->receive(client_msg)) {
    bool server_response_sent_join;
    if (connInfo->curr_connection->get_last_result() == Connection::INVALID_MSG) {
      server_response_msg.tag = TAG_ERR;
      server_response_msg.data = "Invalid message";
      server_response_sent_join = connInfo->curr_connection->send(server_response_msg);
    } else {
      server_response_msg.tag = TAG_ERR;
      server_response_msg.data = "couldn't receive message";
      server_response_sent_join = connInfo->curr_connection->send(server_response_msg);
    }
    if (server_response_sent_join == false){
      std::cerr << "Could not send error message from server to reciever (server could not recieve join msg from reciever)";      
    }
    delete user;
    return;

  } else { // received join message successfully from receiver
    if (client_msg.tag == TAG_JOIN) { 
      // create/find room to put receiver user in
      room = connInfo->server->find_or_create_room(trim(client_msg.data));
      room->add_member(user);
      server_response_msg.tag = TAG_OK;
      server_response_msg.data = "welcome";
      if (!connInfo->curr_connection->send(server_response_msg)) {
        std::cerr << "server could not send response to reciever's join msg";
        room->remove_member(user);
        delete user;
        return;
      }
    } else {
      // join message is invalid - extra error handling
      server_response_msg.tag = TAG_ERR;
      server_response_msg.data = "invalid message";
      if (!connInfo->curr_connection->send(server_response_msg)) {
        std::cerr << "server could not send response to reciever's msg (which should be join msg but is not)";
      }
      // sends error msg successfully
      delete user;
      return;
    }
  }

  while(connInfo->curr_connection->is_open()) {
    Message *message_from_sender = user->mqueue.dequeue();
    if (message_from_sender != nullptr) {
      // send sender message to receiver to be printed in stdout
      if (!connInfo->curr_connection->send(*message_from_sender)) {
        delete message_from_sender;
        break;
      }
      delete message_from_sender;
    }

  }
  // if server exits, need to remove user
  room->remove_member(user);
  delete user;
  return;
}


void chat_with_sender(ConnectionInfo *connInfo,User *user) {
  Room *room = nullptr;
  Message server_response_msg;
  while(connInfo->curr_connection->is_open()) {
    Message client_msg;
    // Message server_response_msg;
    // check if message was recieved by server from sender
    if (!connInfo->curr_connection->receive(client_msg)) {
      bool msg_sent;
      if (connInfo->curr_connection->get_last_result() == Connection::INVALID_MSG || connInfo->curr_connection->get_last_result() == Connection::EOF_OR_ERROR) {
        server_response_msg.tag = TAG_ERR;
        server_response_msg.data = "Invalid message";
        msg_sent = connInfo->curr_connection->send(server_response_msg);
      } else {
        // no tag means no message received or some other error (extra error handling)
        server_response_msg.tag = TAG_ERR;
        server_response_msg.data = "Did not receive message";
        msg_sent = connInfo->curr_connection->send(server_response_msg);
      }
      if (msg_sent == false){
        std::cerr << "Could not send msg from server to sender";
        delete user;
        return;
      }
      continue;
    } else { // message was received
      bool msg_sent;
      // check if message tagged with error (extra error handling)
      if (client_msg.tag == TAG_ERR) {
        std::cerr<< client_msg.data;
        delete user;
        return;
      }

      // message receieved too long
      else if (client_msg.data.length() >= Message::MAX_LEN) {
        server_response_msg.tag = TAG_ERR;
        server_response_msg.data = "Message length exceeds max length";
        msg_sent = connInfo->curr_connection->send(server_response_msg);
        if(msg_sent == false){
          std::cerr << "could not send msg from server to sender - msg "
                       "recieved is too long";
          delete user;
          return;
        }
        continue;
      }

      // check if message tagged with quit
      else if (client_msg.tag == TAG_QUIT) {
        server_response_msg.tag = TAG_OK;
        server_response_msg.data = "Bye";
        msg_sent = connInfo->curr_connection->send(server_response_msg);
        if (msg_sent == false){
          std::cerr << "could not send message from server to sender";
        }
        delete user;
        return;
      }

      // if user not in a room the only message they can send is join or leave
      else if (room == nullptr) {
        // check if message tagged with join
        if (client_msg.tag == TAG_JOIN) {
          room = connInfo->server->find_or_create_room(trim(client_msg.data));
          server_response_msg.tag = TAG_OK;
          server_response_msg.data = "joined room " + client_msg.data;
          if (!connInfo->curr_connection->send(server_response_msg)){
            std::cerr<< "Error sending message from server to sender";
            delete user;
            return;
          }
        } 
        else if (client_msg.tag == TAG_LEAVE) {
          server_response_msg.tag = TAG_ERR;
          server_response_msg.data = "User not in current room";
          if (!connInfo->curr_connection->send(server_response_msg)){
            std::cerr<< "Error sending message from server to sender";
            delete user;
            return;
          }
        } else {
          server_response_msg.tag = TAG_ERR;
          server_response_msg.data = "User must join room to send a message!";
          if (!connInfo->curr_connection->send(server_response_msg)){
            std::cerr<< "Error sending message from server to sender";
            delete user;
            return;
          }
        }
      } else { // sender client in a room
        // check if message tagged with leave
        if (client_msg.tag == TAG_LEAVE) {
          room = nullptr;
          server_response_msg.tag = TAG_OK;
          server_response_msg.data = "left room " + client_msg.data;
          if (!connInfo->curr_connection->send(server_response_msg)){
            std::cerr<< "Error sending message from server to sender";
            delete user;
            return;
          }
        }
        // check if user wants to join a new room
        else if (client_msg.tag == TAG_JOIN) {
          room = connInfo->server->find_or_create_room(trim(client_msg.data));
          server_response_msg.tag = TAG_OK;
          server_response_msg.data = "joined room " + client_msg.data;
          if (!connInfo->curr_connection->send(server_response_msg)){
            std::cerr<< "Error sending message from server to sender";
            delete user;
            return;
          }
        }
        // check if message tagged with delivery
        else if (client_msg.tag == TAG_SENDALL) {
          room->broadcast_message(user->username,client_msg.data);
          server_response_msg.tag = TAG_OK;
          server_response_msg.data = "message sent";
          if (!connInfo->curr_connection->send(server_response_msg)){
            std::cerr<< "Error sending message from server to sender";
            delete user;
            return;
          }
        } else {
          // message tag doesnt exist
          server_response_msg.tag = TAG_ERR;
          server_response_msg.data = "Invalid Tag";
          if (!connInfo->curr_connection->send(server_response_msg)){
            std::cerr<< "Error sending message from server to sender";
            delete user;
            return;
          }
        }
      }
    }
  }
  // if servers exits need to free user and connInfo to prevent memory leaks
  delete user;
  return;
}

namespace {
void handle_receive_error(ConnectionInfo *currInfo) {
  Message server_response_msg;
  if (currInfo->curr_connection->get_last_result() == Connection::INVALID_MSG) {
    server_response_msg.tag = TAG_ERR;
    server_response_msg.data = "invalid message";
    if (!currInfo->curr_connection->send(server_response_msg)) {
      std::cerr << "Could not send message.";
    }
  } else {
    server_response_msg.tag = TAG_ERR;
    server_response_msg.data = "couldn't receive message";
    if (!currInfo->curr_connection->send(server_response_msg)) {
      std::cerr << "Could not send message.";
    }
  }
}

bool handle_send(Message client_msg, std::string *username, ConnectionInfo *currInfo) {
  Message server_response_msg;
  if (client_msg.tag == TAG_RLOGIN || client_msg.tag == TAG_SLOGIN) {
    // extract username from login message payload
    *username = trim(client_msg.data);
    server_response_msg.tag = TAG_OK;
    server_response_msg.data = "logged in as " + *username;
    if (!currInfo->curr_connection->send(server_response_msg)) {
      std::cerr << "could not send response to login message to client";
      return false;
    }
  } else { // message tagged with another tag instead of login
    server_response_msg.tag = TAG_ERR;
    server_response_msg.data = "No login message recieved by server";
    if (!currInfo->curr_connection->send(server_response_msg)) {
      std::cerr << "could not send response to first message to client";
      return false;
    }
    return false;
  }
  return true;
}

void *worker(void *arg) {
  pthread_detach(pthread_self());
  struct ConnectionInfo *currInfo = (ConnectionInfo *)arg;
  // read and respond to client login message
  std::string username = "";
  Message client_msg;

  // handle error receiving login response from client
  if (!currInfo->curr_connection->receive(client_msg)) {
    handle_receive_error(currInfo);
    return nullptr;
  } else { // received login message from client
    if (!handle_send(client_msg, &username, currInfo)) {
      return nullptr;
    }
  }
  // create user using its username
  User *user = new User(username);
  // check if client joined as sender or receiver
  if (client_msg.tag == TAG_RLOGIN) {
    chat_with_receiver(currInfo,user);
  } else { 
    chat_with_sender(currInfo,user);
  }
  return nullptr;
}
}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  pthread_mutex_init(&m_lock, nullptr);
}

Server::~Server() {
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // use open_listenfd to create the server socket/fd, return true
  // if successful, false if not
  m_ssock = open_listenfd(std::to_string(m_port).c_str());
  if (m_ssock < 0) {
    std::cerr << "Couldn't open server socket";
    return false;
  }
  return true;
}

void Server::handle_client_requests() {
  while (1) {
    // call accept to accept/establish a connection with the client
    int client_socket = Accept(m_ssock, nullptr, nullptr);
    if (client_socket < 0) {
        std::cerr << "Error accepting client connection";
        return;
		}

		// create ConnInfo object to manage communication with client through client socket/fd
    struct ConnectionInfo *connInfo = new ConnectionInfo();
      connInfo->curr_connection = new Connection(client_socket);
      connInfo->server = this;

    // create a thread for each client connection using pthread_create()
		pthread_t thr_id;
		if (pthread_create(&thr_id, NULL, worker, connInfo) != 0) {
			std::cerr<<"pthread_create failed";
      return;
		}
	}
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // Return a pointer to the unique Room object representing  the named chat room, creating a new one if necessary
  {
    Guard g(m_lock);
    auto i = m_rooms.find(room_name);

    if (i != m_rooms.end()) {
      return i->second;
    } else {
      Room *new_room = new Room(room_name);
      m_rooms[room_name] = new_room;
      return new_room;
    }
  }
}