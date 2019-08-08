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
            this, SLOT(readyRead()));

    qDebug() << "connecting...";

    // this is not blocking call
    socket->connectToHost(QString::fromStdString(address), port);

    // we need to wait... timeout is hardcoded here
    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
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
    qDebug() << bytes << " bytes written...";
}

void ClientSocket::readyRead()
{
    qDebug() << "reading...";

    while(socket->bytesAvailable() > 0){
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
        emit incomingMessage(m);
        //qDebug() << "->-> message processed.";
    }
    //qDebug() << "-------------------------------- Now no more messages! Nice!";
}

void ClientSocket::writeData(char *data, int len){
    qDebug() << "writing " << len << " bytes ...";
    int written = socket->write(data, len);
    qDebug() << written << " bytes written.";
}
