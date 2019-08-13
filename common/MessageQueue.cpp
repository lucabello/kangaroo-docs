#include "MessageQueue.h"

int MessageQueue::count(){
    QMutexLocker locker(&mutex);
    int count = queue.count();
    return count;
}

bool MessageQueue::isEmpty(){
    QMutexLocker locker(&mutex);
    bool empty = queue.isEmpty();
    return empty;
}

void MessageQueue::clear(){
    QMutexLocker locker(&mutex);
    queue.clear();
}

void MessageQueue::push(const Message& m){
    QMutexLocker locker(&mutex);
    queue.enqueue(m);
    cv.notify_all();
}

Message MessageQueue::pop(){
    QMutexLocker locker(&mutex);
    while(queue.isEmpty())
        cv.wait(locker.mutex());
    Message m = queue.dequeue();
    return m;
}
