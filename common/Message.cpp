//
// Created by lucab on 06/05/2019.
//

#include "Message.h"

Message::Message() {};

Message::Message(MessageType t, Symbol s) : type(t), value(s) {};

Message::Message(MessageType t, std::string cmd) : type(t), command(cmd) {};

MessageType Message::getType() const {
    return type;
}

Symbol Message::getSymbol() const {
    return value;
}

std::string Message::toString(){
    std::string result = "Message: ";
    if(this->type == MessageType::Insert)
        result += "Insert";
    else if(this->type == MessageType::Erase)
        result += "Erase";
    else if(this->type == MessageType::Login)
        result += "Login";
    else if(this->type == MessageType::Open)
        result += "Open";
    else if(this->type == MessageType::Error)
        result += "Error";
    else if(this->type == MessageType::Create)
        result += "Create";
    else if(this->type == MessageType::FileList)
        result += "FileList";
    else if(this->type == MessageType::Register)
        result += "Register";
    else
        result += "Unknown";
    result += " - Content: ";
    if(this->type != MessageType::Erase &&this->type!=MessageType::Insert)
        result += this->command;
    else
        result += this->value.toString();
    return result;
}

std::string Message::getCommand() const {
    return command;
}

char* Message::serialize(Message m){
    char *bytes = new char[100];
    int offset = 0;
    Symbol::pushIntToByteArray(m.type, bytes, &offset);
    if(m.type == MessageType::Erase||m.type==MessageType::Insert)
        Symbol::pushObjectIntoArray(m.value, bytes, &offset);
    else {
        Symbol::pushIntToByteArray(m.command.length(), bytes, &offset);
        const char *cstring = m.command.c_str();
        for(int i=0; i<m.command.length(); i++){
            bytes[offset] = cstring[i];
            offset++;
        }
    }
    return bytes;
}

Message Message::unserialize(const char *bytes){
    Message m;
    int offset = 0;
    m.type = (MessageType)Symbol::popIntFromByteArray(bytes, &offset);
    if(m.type == MessageType::Erase||m.type==MessageType::Insert)
        m.value = Symbol::popObjectFromArray(bytes, &offset);
    else {
        char cstring[100];
        int length = Symbol::popIntFromByteArray(bytes, &offset);
        int i;
        for(i=0; i<length; i++)
            cstring[i] = bytes[i+offset];
        cstring[i] = '\0';
        m.command = std::string(cstring);
    }
    return m;
}
