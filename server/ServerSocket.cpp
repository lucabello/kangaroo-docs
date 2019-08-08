#include "ServerSocket.h"

ServerSocket::ServerSocket(QObject *parent) :
    QObject(parent)
{
}

ServerSocket::ServerSocket(QTcpSocket *s): socket(s),
    QObject(nullptr)
{
    if(s != nullptr)
        descriptor = s->socketDescriptor();
    else
        descriptor = -100;
    connect(socket, SIGNAL(connected()),
            this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(readyRead()));
}

void ServerSocket::processMessage(Message m){
    //qDebug() << "Processing following message: ";
    //qDebug() << QString::fromStdString(m.toString());
    if(m.getType() == MessageType::Command){
        emit controlCommand(descriptor, m.getCommand());
    }
    else if(m.getType() == MessageType::Insert){
        emit actionCommand(descriptor, m);

    }
    else if(m.getType() == MessageType::Erase){
        emit actionCommand(descriptor, m);
    }
}

int ServerSocket::getDescriptor(){
    return descriptor;
}

void ServerSocket::connected()
{
    qDebug() << socket->socketDescriptor() << " connected...";
}

void ServerSocket::disconnected()
{
    qDebug() << descriptor << "disconnected...";
}

void ServerSocket::bytesWritten(qint64 bytes)
{
    qDebug() << "[DEBUG] " << bytes << " bytes written...";
}

void ServerSocket::readyRead()
{
    while(socket->bytesAvailable() > 0){
        //qDebug() << "reading from" << socket->socketDescriptor() << "...";
        while(socket->bytesAvailable() < 8)
            qDebug() << " >> Waiting for more than 8 bytes...";
        //qDebug() << "... more than 8 bytes available";
        char *header = socket->read(8).data();
        int payloadLength = Symbol::peekIntFromByteArray(header+4); //peek length, not message type
        //qDebug() << "... payloadLength = " << payloadLength;
        //qDebug() << "... bytesAvailable = " << socket->bytesAvailable();

        while(socket->bytesAvailable() < payloadLength)
            qDebug() << " >> Waiting for more than 8 bytes...";

        char *payload = socket->read(payloadLength).data();
        char bytes[100];
        for(int i=0; i<8; i++)
            bytes[i] = header[i];
        for(int i=0; i<payloadLength; i++)
            bytes[i+8] = payload[i];
        Message m = Message::unserialize(bytes);
        //qDebug() << "NEW MESSAGE";
        //qDebug() << "->-> MessageType: " << m.getType();
        //qDebug() << "->-> SymbolContent: " << m.getSymbol().getContent();
        //qDebug() << "->-> processing message...";
        processMessage(m);
        //qDebug() << "->-> message processed.";
    }
    //qDebug() << "-------------------------------- Now no more messages! Nice!";
}

void ServerSocket::writeData(char *data, int len){
    qDebug() << "[DEBUG] writing data to" << socket->socketDescriptor() << " ...";
    qDebug() << "[DEBUG] writing " << len << " bytes ...";
    int written = socket->write(data, len);
}
