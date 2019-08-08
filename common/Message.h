//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_MESSAGE_H
#define KANGAROO_DOCS_MESSAGE_H

#include "Symbol.h"
enum MessageType {Insert, Erase, Command};

/**
 * It holds information for the communication between Editor and Server
 */
class Message {
    MessageType type;
    Symbol value;
    std::string command;
public:
    //Default constructor
    Message();
    //Constructor for Insert or Erase, specifying the symbol
    Message(MessageType t, Symbol s);
    //Constructor for Command, specifying the command string
    Message(MessageType t, std::string cmd);

    MessageType getType() const;
    Symbol getSymbol() const;
    /**
     * Returns a human-readable string with all symbol relevant properties,
     * to check for correctness when debugging the application.
     *
     * @return a readable digest of the symbol
     */
    std::string toString();
    std::string getCommand() const;
    //first 4 bytes: message type - second 4 bytes: remaining length
    static char* serialize(Message m);
    static Message unserialize(const char *bytes);

};


#endif //KANGAROO_DOCS_MESSAGE_H
