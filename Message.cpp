//
// Created by lucab on 06/05/2019.
//

#include "Message.h"

Message::Message() {};

Message::Message(MessageType t, Symbol s) : type(t), value(s) {};

MessageType Message::getType() const {
    return type;
}

Symbol Message::getSymbol() const {
    return value;
}