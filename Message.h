//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_MESSAGE_H
#define KANGAROO_DOCS_MESSAGE_H

#include "Symbol.h"

enum MessageType {INSERT = 0, ERASE = 1, FORMAT = 2, LOCAL = 3};

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
};


#endif //KANGAROO_DOCS_MESSAGE_H
