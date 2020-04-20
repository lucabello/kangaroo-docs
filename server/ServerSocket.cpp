#include "ServerSocket.h"

ServerSocket::ServerSocket(QObject *parent) :
    QObject(parent)
{
}

ServerSocket::ServerSocket(QTcpSocket *s): QObject(nullptr),socket(s)
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
            this, SLOT(readMessage()));
}

qintptr ServerSocket::getDescriptor(){
    return descriptor;
}

void ServerSocket::connected()
{
    qDebug() << socket->socketDescriptor() << " connected...";
    qDebug() << "[ServerSocket] socket state "<< socket->state();
}

void ServerSocket::disconnected()
{
    qDebug() << descriptor << "disconnected...";
    hostDisconnected(descriptor);
    qDebug() << "[ServerSocket] socket state "<< socket->state();
}

void ServerSocket::bytesWritten(qint64 bytes)
{
    qDebug() << "[DEBUG] " << bytes << " bytes written...";
}

void ServerSocket::readMessage()
{
    QDataStream clientReadStream(socket);
    qint32 nextMessageSize=0;
    Message message;
    while(socket->bytesAvailable() > 0) {
        while(socket->bytesAvailable()<4)
            qDebug() << " >> Waiting for the next message size..";
        clientReadStream >> nextMessageSize;
        while(socket->bytesAvailable() < nextMessageSize)
            qDebug() << " >> Waiting for the next message of size "+QString::fromStdString(std::to_string(nextMessageSize))+"...";

        clientReadStream >> message;

        qDebug() << "[ServerSocket] I read this message state "<< QString::fromStdString(message.toString());
        qDebug() << "[ServerSocket] socket state "<< socket->atEnd();
        qDebug() << "[ServerSocket] socket state "<< socket->state();

        signalMessage(descriptor, message);
    }
}

void ServerSocket::writeMessage(Message message){
    qDebug() << "[ServerSocket] socket state "<< socket->state();
    if(!isConnected())
        return;

    qDebug() << "[ServerSocket] I write this message: " << QString::fromStdString(message.toString());

    //SERIALIZATION
    QByteArray serializedMessage;
    QDataStream serializedStream(&serializedMessage, QIODevice::ReadWrite);
    serializedStream << message;

    //NETWORK -- ADDING SIZE
    QByteArray networkMessage;
    qint32 size=serializedMessage.size();
    QDataStream networkStream(&networkMessage, QIODevice::ReadWrite);
    networkStream << size << message;

    qDebug() << size << " + " << sizeof (size) << " = " << networkMessage.size() << "bytes sending";
    qint64 written = 0;
    int count = 0;
    while(written < size && isConnected()) {
        written = socket->write(networkMessage);
        count++;
        qDebug() << written << " bytes written.";
    }
    socket->flush();
}

void ServerSocket::disconnectFromHost(){
    socket->disconnectFromHost();
}

bool ServerSocket::isConnected(){
    return socket->state() == QTcpSocket::ConnectedState;
}
