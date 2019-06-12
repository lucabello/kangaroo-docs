#include "mytcpsocket.h"

MyTcpSocket::MyTcpSocket(QObject *parent) :
    QObject(parent)
{
}

void MyTcpSocket::doConnect()
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
    socket->connectToHost("172.16.135.87", 1501);

    // we need to wait...
    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void MyTcpSocket::connected()
{
    qDebug() << "connected...";
}

void MyTcpSocket::disconnected()
{
    qDebug() << "disconnected...";
}

void MyTcpSocket::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void MyTcpSocket::readyRead()
{
    qDebug() << "reading...";

    // read the data from the socket
    qDebug() << socket->readAll();
}

void MyTcpSocket::writeData(QString s){
    qDebug() << "writing " << s << " ...";
    socket->write(s.toLocal8Bit());
}
