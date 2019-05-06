//
// Created by lucab on 06/05/2019.
//

#include <algorithm>
#include "NetworkServer.h"

int NetworkServer::connect(SharedEditor *sharedEditor) {
    connectedEditors.push_back(sharedEditor);
    return nextSiteId++;
}

//Using the erase-remove idiom, all occurrences of sharedEditor are removed
void NetworkServer::disconnect(SharedEditor *sharedEditor) {
    connectedEditors.erase(std::remove_if(connectedEditors.begin(),
            connectedEditors.end(),
            [sharedEditor](auto e){return e == sharedEditor;}),
                    connectedEditors.end());
}

void NetworkServer::send(const Message &m) {
    messageQueue.push(m);
}

void NetworkServer::dispatchMessages() {

}