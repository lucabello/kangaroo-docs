//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_MESSAGE_H
#define KANGAROO_DOCS_MESSAGE_H

#include "Symbol.h"
#include "Serializable.h"

enum MessageType {Insert, Erase, Command};

/**
 * It holds information for the communication between Editor and Server
 */
class Message {
    MessageType type;
    Symbol value;
public:
    Message();
    Message(MessageType t, Symbol s);

    MessageType getType() const;
    Symbol getSymbol() const;
    static char* serialize(Message m);
    static Message unserialize(char *bytes);

};


#endif //KANGAROO_DOCS_MESSAGE_H
