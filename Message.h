//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_MESSAGE_H
#define KANGAROO_DOCS_MESSAGE_H

#include "Symbol.h"

enum Type {INSERT = 0, ERASE = 1};

/**
 * It holds information for the communication between Editor and Server
 */
class Message {
    Type messageType;
    Symbol value;
public:
    Message();
    Message(Type t, Symbol s);
};


#endif //KANGAROO_DOCS_MESSAGE_H
