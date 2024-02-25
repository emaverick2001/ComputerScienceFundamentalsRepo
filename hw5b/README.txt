Sample README.txt

Students:
Rupa Chalavadi (rchalav1)
Maverick Espinosa (mespin11)

Please include where your critical sections are,
 how you determined them, and why you chose the 
 synchronization primitives for each section. 
 You should also explain how your critical sections 
 ensure that the synchronization requirements are met 
 without introducing synchronization hazards 
 (e.g. race conditions and deadlocks).

Our critical sections are within message_queue.cpp, and room.cpp.
We were able to localize them to these two classes since
they are the only data structures utilized by threads, meaning there is a possibility for data race
conditions to arise if multiple threads access them synchronously. Since 
each client(receiver or sender) has their own respective message_queue and are placed in a room
based on their join message, the functions handling these items are accessed by threads and
so we need to enforce mutual exclusion between the threads when they access either data structure. In 
message_queue.cpp, the critical sections involve the code that interacts with the 
data structure and modifies elements (i.e. enqueue and dequeue). We want to make sure 
that only one thread is accessing these functions at a time or else the contents of the queue 
will be out of sync with the actual contents they are supposed to contain.
In room.cpp, the crticial sections also involve any code that interacts and changes elements within
the room datastructure (a set of User objects). These are the add_member, remove_member, and the 
broadcast_message functions. The broadcast message function is also included since it's dealing 
with handling Receiver User messages, thus its interacting with a message_queue data structure 
and needs to be protected from multiple thread accesses. For both classes we chose to use
Guard objects in order to protect the sections since the Guard objects ensure that the 
critical seciton will always be unlocked after being locked since their scope is limited to
only the critical section utilizing the data structure.