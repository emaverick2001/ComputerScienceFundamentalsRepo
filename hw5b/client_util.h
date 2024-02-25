#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#include <string>
class Connection;
struct Message;

// this header file is useful for any declarations for functions
// or classes that are used by both clients (the sender and receiver)

std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

// you can add additional declarations here...
bool handle_send(Connection *conn, std::string TAG, std::string msg_data);
bool handle_response(Connection *conn);
void handle_TAG_DELIVERY(Message msg);
bool isAlphanumeric(const std::string &str);
bool arg_check(int server_port, std::string username);
bool handle_response_no_quit(Message msg, Connection *conn,
                             std::string *response_tag);
bool get_msg(Message *msg);

#endif // CLIENT_UTIL_H
