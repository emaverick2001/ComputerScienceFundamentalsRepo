// communication loops for the senders and receivers
// bool handle_receive_error_no_quit(ConnectionInfo *connInfo, User *user) {
//   bool msg_sent;
//   Message server_response_msg;
//   if (connInfo->curr_connection->get_last_result() == Connection::INVALID_MSG
//   ||
//       connInfo->curr_connection->get_last_result() ==
//           Connection::EOF_OR_ERROR) {
//     server_response_msg.tag = TAG_ERR;
//     server_response_msg.data = "Invalid message";
//     msg_sent = connInfo->curr_connection->send(server_response_msg);
//   } else {
//     // no tag means no message received or some other error (extra error
//     // handling)
//     server_response_msg.tag = TAG_ERR;
//     server_response_msg.data = "Did not receive message";
//     msg_sent = connInfo->curr_connection->send(server_response_msg);
//   }
//   if (msg_sent == false) {
//     std::cerr << "Could not send msg from server.";
//     delete user;
//     return false;
//   }
//   return true;
// }

// bool check_error_response(Message client_msg, User *user, ConnectionInfo
// *connInfo, bool *cont) {
//   Message server_response_msg;
//   // check if message tagged with error (extra error handling)
//   if (client_msg.tag == TAG_ERR) {
//     std::cerr << client_msg.data;
//     delete user;
//     return false;
//   }

//   // message receieved too long
//   else if (client_msg.data.length() >= Message::MAX_LEN) { // CHANGED THIS
//   HERE
//     server_response_msg.tag = TAG_ERR;
//     server_response_msg.data = "Message length exceeds max length";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "could not send msg from server to sender - msg recieved
//       is "
//                    "too long";
//       delete user;
//       return false;
//     }
//    *cont = true;
//   }

//   // check if message tagged with quit
//   else if (client_msg.tag == TAG_QUIT) {
//     server_response_msg.tag = TAG_OK;
//     server_response_msg.data = "Bye";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "could not send message from server to sender";
//     }
//     delete user;
//     return false;
//   }
//   return true;
// }

// bool handle_user_not_in_room(Message client_msg, Room *room, ConnectionInfo
// *connInfo, User *user) {
//   Message server_response_msg;
//   // check if message tagged with join
//   if (client_msg.tag == TAG_JOIN) {
//     room = connInfo->server->find_or_create_room(trim(client_msg.data));
//     server_response_msg.tag = TAG_OK;
//     server_response_msg.data = "joined room " + client_msg.data;
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "Error sending message from server to sender";
//       delete user;
//       return false;
//     }
//   } else if (client_msg.tag == TAG_LEAVE) {
//     server_response_msg.tag = TAG_ERR;
//     server_response_msg.data = "User not in current room";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "Error sending message from server to sender";
//       delete user;
//       return false;
//     }
//   } else {
//     server_response_msg.tag = TAG_ERR;
//     server_response_msg.data = "User must join room to send a message!";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "Error sending message from server to sender";
//       delete user;
//       return false;
//     }
//   }
//   return true;
// }

// bool handle_sender_in_room(Message client_msg, Room *room, ConnectionInfo
// *connInfo, User *user) {
//   Message server_response_msg;
//   if (client_msg.tag == TAG_LEAVE) {
//     room = nullptr;
//     server_response_msg.tag = TAG_OK;
//     server_response_msg.data = "left room " + client_msg.data;
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "Error sending message from server to sender";
//       delete user;
//       return false;
//     }
//   }
//   // check if user wants to join a new room
//   else if (client_msg.tag == TAG_JOIN) {
//     room = connInfo->server->find_or_create_room(trim(client_msg.data));
//     server_response_msg.tag = TAG_OK;
//     server_response_msg.data = "joined room " + client_msg.data;
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "Error sending message from server to sender";
//       delete user;
//       return false;
//     }
//   }
//   // check if message tagged with delivery
//   else if (client_msg.tag == TAG_SENDALL) {
//     room->broadcast_message(user->username, client_msg.data);
//     server_response_msg.tag = TAG_OK;
//     server_response_msg.data = "message sent";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "Error sending message from server to sender";
//       delete user;
//       return false;
//     }
//   } else {
//     // message tag doesnt exist
//     server_response_msg.tag = TAG_ERR;
//     server_response_msg.data = "Invalid Tag";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "Error sending message from server to sender";
//       delete user;
//       return false;
//     }
//   }

//   return true;
// }

// bool handle_server_response_receiver(Message client_msg, User *user, Room
// *room,
//                                      ConnectionInfo *connInfo) {
//   Message server_response_msg;
//   if (client_msg.tag == TAG_JOIN) {
//     // create/find room to put receiver user in
//     room = connInfo->server->find_or_create_room(trim(client_msg.data));
//     room->add_member(user);
//     server_response_msg.tag = TAG_OK;
//     server_response_msg.data = "welcome";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "server could not send response to reciever's join msg";
//       room->remove_member(user);
//       delete user;
//       return false;
//     }

//   } else {
//     // join message is invalid - extra error handling
//     server_response_msg.tag = TAG_ERR;
//     server_response_msg.data = "invalid message";
//     if (!connInfo->curr_connection->send(server_response_msg)) {
//       std::cerr << "server could not send response to reciever's msg (which "
//                    "should be join msg but is not)";
//     }
//     // sends error msg successfully
//     delete user;
//     return false;
//   }
//   return true;
// }