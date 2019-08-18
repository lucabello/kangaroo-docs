//
// Created by lucab on 06/05/2019.
//

#include "Message.h"

Message::Message() {};

Message::Message(MessageType t, Symbol s) : type(t), value(s) {};

Message::Message(MessageType t, QString cmd) : type(t), command(cmd) {};

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
    else if(this->type == MessageType::EditorList)
        result += "Editor List";
    else if(this->type == MessageType::URI)
        result += "URI";
    else
        result += "Unknown";
    result += " - Content: ";
    if(this->type != MessageType::Erase &&this->type!=MessageType::Insert)
        result += this->command.toStdString();
    else
        result += this->value.toString();
    return result;
}

QString Message::getCommand() const {
    return command;
}

QDataStream& operator<<(QDataStream &out, const Message &item){
    if(item.type==MessageType::Insert||item.type==MessageType::Erase)
        out << item.type << item.value;
    else
        out << item.type << item.command;
    return out;
}
QDataStream &operator>>(QDataStream &in, Message &item){
    qint32 messageType;
    Symbol symbol;

    in >> messageType;
    switch(messageType){
        case MessageType::Erase:
        case MessageType::Insert:
        {
            in >> symbol;
            item = Message{static_cast<MessageType>(messageType),symbol};
            break;
        }
        default:
            QString command;
            in >> command;
            item = Message{static_cast<MessageType>(messageType),command};
            break;
    }
    return in;
}
