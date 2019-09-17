#include "ClientSocket.h"

ClientSocket::ClientSocket(std::string a, unsigned short p): address(a), port(p),
    QObject(nullptr)
{
}

void ClientSocket::doConnect()
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()),
            this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(readMessage()));

    qDebug() << "connecting...";

    // this is not blocking call
    socket->connectToHost(QString::fromStdString(address), port);

    // we need to wait... timeout is hardcoded here
    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void ClientSocket::doDisconnect(){
    socket->disconnectFromHost();
}

void ClientSocket::connected()
{
    qDebug() << "connected...";
}

void ClientSocket::disconnected()
{
    qDebug() << "disconnected...";
}

void ClientSocket::bytesWritten(qint64 bytes)
{
    //qDebug() << bytes << " bytes written...";
}

void ClientSocket::readMessage()
{
    QDataStream clientReadStream(socket);
    qint32 nextMessageSize=0;
    Message message;
    while(socket->bytesAvailable() > 0) {
        while(socket->bytesAvailable()<4)
            qDebug() << " >> Waiting for the next message size..";
        clientReadStream >> nextMessageSize;
        while(socket->bytesAvailable() < nextMessageSize)
            qDebug() << " >> Waiting for the next message...";

        clientReadStream >> message;
        qDebug() << "[ClientSocket] I read this message: " << QString::fromStdString(message.toString());
        message.getType();

        signalMessage(message);
    }
}

void ClientSocket::writeMessage(Message message){
    qDebug() << "[ClientSocket] I write this message: " << QString::fromStdString(message.toString());

    //SERIALIZATION
    QByteArray serializedMessage;
    QDataStream serializedStream(&serializedMessage, QIODevice::ReadWrite);
    serializedStream << message;

    //NETWORK -- ADDING SIZE
    QByteArray networkMessage;
    qint32 size=serializedMessage.size();
    QDataStream networkStream(&networkMessage, QIODevice::ReadWrite);
    networkStream << size << message;
/*
    qDebug() << "[ClientSocket] Sending " << size << " bytes for this message:";
    qDebug() << QString::fromStdString(message.toString());
*/
    qDebug() << size << " + " << sizeof (size) << " = " << networkMessage.size() << "bytes sending";
    qint64 written = socket->write(networkMessage);
    socket->flush();
    qDebug() << written << " bytes written.";
}

std::string ClientSocket::getaddress()
{
    return address;
}
