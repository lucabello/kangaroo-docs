#include "KangarooServer.h"

KangarooServer::KangarooServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

KangarooServer::KangarooServer(std::string a, unsigned short p) :
    QObject(nullptr)
{
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    QHostAddress addr;
    addr.setAddress(QString::fromStdString(a));
    if(!server->listen(addr, p))
    {
        qDebug() << "[ERR] Server could not start";
    }
    else
    {
        qDebug() << "[SERVER] Server started - " << QString::fromStdString(a) << ":" << p;
    }
}

void KangarooServer::newConnection()
{
    qDebug() << "[SERVER] Connection received!";
    ServerSocket *mySocket = new ServerSocket(server->nextPendingConnection());

    connect(mySocket, &ServerSocket::controlCommand,
            this, &KangarooServer::control);
    connect(mySocket, &ServerSocket::actionCommand,
            this, &KangarooServer::action);

    int descriptor = mySocket->getDescriptor();
    ConnectedEditor ce(mySocket);
    std::pair<int,ConnectedEditor> pair(descriptor, ce);
    descriptorToEditor.insert(pair);
    //TEMP FOR TESTING ONLY
    connectedList.push_back(descriptor);
    qDebug() << "[SERVER] Editor info created for " << descriptor << "!";
    //mySocket->writeData(QString::fromStdString("Welcome, beautiful client " + std::to_string(descriptor) + ".\r\n"));
//    socket->flush();
//    socket->waitForBytesWritten(3000);
//    socket->close();
}

void KangarooServer::control(int descriptor, std::string message){
    qDebug() << "(" << descriptor << ") C: " << QString::fromStdString(message);
}

void KangarooServer::action(int descriptor, Message m){
    qDebug() << "A: " << QString::fromStdString(m.toString());
    //TEMP FOR TESTING ONLY
    char *serM = Message::serialize(m);
    int len = Symbol::peekIntFromByteArray(serM+4)+8;
    for(int d : connectedList){
        if(d != descriptor){
            descriptorToEditor.at(d).getSocket()->writeData(serM, len);
        }
    }
}
