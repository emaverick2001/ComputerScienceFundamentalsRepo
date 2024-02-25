#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // initialize the mutex when room datastructure created
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  // destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // add User to the room
  {
    Guard g(lock);
    members.insert(user);
  }
  
}

void Room::remove_member(User *user) {
  // remove User from the room
  {
    Guard g(lock);
    members.erase(user);
  }
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // Send a message to every (receiver) User in the room
  {
    Guard g(lock);

    for (auto u : members) {
      // traverse through members that are not the sender
      if (u->username != sender_username) {
        Message *msg = new Message();
        msg->tag = TAG_DELIVERY;
        msg->data = this->get_room_name() + ":" + sender_username + ":" + message_text;

        // enqueue message in each receiver clients mqueue
        u->mqueue.enqueue(msg);
      }
    }
  }
}
