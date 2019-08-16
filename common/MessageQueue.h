#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include "Message.h"

/*
 * Implementation of a thread safe queue.
 * NEEDS TESTING!!
 */
class MessageQueue {
    QQueue<Message> queue;
    QWaitCondition cv;
    QMutex mutex;
public:
    MessageQueue(){}

    /**
     * Returns the number of elements in the QQueue<Message> of the class
     */
    int count();

    /**
     * Returns a boolean that says if the QQueue<Message> is empty
     */
    bool isEmpty();

    /**
     * Removes all the elements inside the QQueue<Message>
     */
    void clear();

    /**
     * Pushes the Message passed as a parameter at the end of the FIFO queue of messages
     */
    void push(const Message& m);

    /**
     * Returns the first Message at the head of the FIFO queue of messages
     */
    Message pop();
};

#endif // MESSAGEQUEUE_H
