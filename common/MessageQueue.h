#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <queue>
#include "Message.h"

/*
 * Implementation of a thread safe queue.
 * Still needs to be implemented, only interface is being defined now.
 */
class MessageQueue {
    std::queue<Message> q;
public:
    MessageQueue();
    //operations
};

#endif // MESSAGEQUEUE_H
