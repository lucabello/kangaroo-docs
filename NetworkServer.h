//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_NETWORKSERVER_H
#define KANGAROO_DOCS_NETWORKSERVER_H

#include <vector>
#include <queue>
#include "Message.h"

class SharedEditor; //forward declaration
/**
 * Class simulating a network server. It keeps information on connected
 * editors and allows them to share messages serving as a centralized
 * controller.
 */
class NetworkServer {
    std::vector<SharedEditor*> connectedEditors;
    std::queue<Message> messageQueue;
    int nextSiteId;
public:
    /**
     * Initialize a NetworkServer with nextSiteId=0
     */
    NetworkServer() : nextSiteId(1){}
    /**
     * Save the pointer to the editor and return
     * a unique identifier
     *
     * @param sharedEditor
     * @return
     */
    int connect(SharedEditor* sharedEditor);
    /**
     * Delete the pointer to the editor
     *
     * @param sharedEditor
     */
    void disconnect(SharedEditor* sharedEditor);
    /**
     * Add a message to a sending queue
     *
     * @param m
     */
    void send(const Message& m);
    /**
     * Dispatch all queued messages to all connected
     * editors except for the message sender
     */
    void dispatchMessages();
};


#endif //KANGAROO_DOCS_NETWORKSERVER_H
