#include "KangarooServer.h"
#include <fstream>

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

    connect(mySocket, &ServerSocket::deliverMessage,
            this, &KangarooServer::processMessage);

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

void KangarooServer::processMessage(int descriptor,Message message){
    switch(message.getType()){
        case MessageType::Erase:
        case MessageType::Insert:
            propagate(descriptor,message);
            break;
        case MessageType::Login:
            login(descriptor,message);
            break;
    }
}

void KangarooServer::propagate(int descriptor, Message message){
    qDebug() << "A: " << QString::fromStdString(message.toString());
    //TEMP FOR TESTING ONLY
    char *serM = Message::serialize(message);
    int len = Symbol::peekIntFromByteArray(serM+4)+8;
    for(int d : connectedList){
        if(d != descriptor){
            descriptorToEditor.at(d).getSocket()->writeData(serM, len);
        }
    }
}

void KangarooServer::login(int descriptor,Message message){
    //open file and look for the tuple, for now everything is fine
    std::vector<std::string> fields=Message::split(message.getCommand(),",");
    std::string username=Message::split(fields.at(0),":").at(1);
    std::string password=Message::split(fields.at(1),":").at(1);
    std::ifstream file("users.txt");
    std::string str;
    bool result=false;
    while (std::getline(file, str)) {
        std::vector<std::string> fields=Message::split(str,",");
        std::string user=fields[0];
        std::string pass=fields[1];
        if(username==user){
            if(password==pass)
                result=true;
            break;
        }
    }
    std::string content;
    if(result)
        content="true";
    else
        content="false";
    Message m {MessageType::Login,content}; //prepare and send message
    char *serM = Message::serialize(m);
    descriptorToEditor.at(descriptor).getSocket()->writeData(serM,Symbol::peekIntFromByteArray(serM+4)+8);
    descriptorToEditor.at(descriptor).getSocket()->disconnectFromHost();//?

}
