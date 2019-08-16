//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_MESSAGE_H
#define KANGAROO_DOCS_MESSAGE_H

#include "Symbol.h"
#include <QDataStream>

enum MessageType {Insert, Erase, Login, Register, FileList, Create, Open, Error};

/**
 * It holds information for the communication between Editor and Server
 */
class Message {
    MessageType type;
    Symbol value;
    QString command;
public:
    //Default constructor
    Message();
    //Constructor for Insert or Erase, specifying the symbol
    Message(MessageType t, Symbol s);
    //Constructor for Command, specifying the command string
    Message(MessageType t, QString cmd);

    MessageType getType() const;
    Symbol getSymbol() const;
    /**
     * Returns a human-readable string with all symbol relevant properties,
     * to check for correctness when debugging the application.
     *
     * @return a readable digest of the symbol
     */
    std::string toString();
    QString getCommand() const;

    friend QDataStream &operator<<(QDataStream &out, const Message &item);
    friend QDataStream &operator>>(QDataStream &in, Message &item);

};


#endif //KANGAROO_DOCS_MESSAGE_H
