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

char* Message::serialize(Message m){
    char *bytes = new char[100];
    int offset = 0;
    Symbol::pushIntToByteArray(m.type, bytes, &offset);
    Symbol::pushObjectIntoArray(m.value, bytes, &offset);
    return bytes;
}

Message Message::unserialize(char *bytes){
    Message m;
    int offset = 0;
    m.type = (MessageType)Symbol::popIntFromByteArray(bytes, &offset);
    m.value = Symbol::popObjectFromArray(bytes, &offset);
    return m;
}
