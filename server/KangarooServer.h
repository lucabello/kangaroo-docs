#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include "ConnectedEditor.h"
#include "ServerSocket.h"
#include "../common/Message.h"

class KangarooServer : public QObject
{
    Q_OBJECT
public:
    explicit KangarooServer(QObject *parent = nullptr);
    explicit KangarooServer(std::string address, unsigned short port);

signals:

public slots:
    void newConnection();
    void incomingMessage(int descriptor,Message message);
    void hostDisconnected(int descriptor);

private:
    QTcpServer *server;
    std::map<int,ConnectedEditor> descriptorToEditor;
    std::map<std::string, std::vector<int>> filenameToDescriptors;
    std::map<std::string, std::vector<Symbol>> filenameToSymbols;
    void modifyFileVector(const Message& m, std::vector<Symbol>& _symbols);
    //Temporary, for testing and debugging collaborative editing
    std::vector<int> connectedList;
    void propagate(int descriptor, Message message);
    void doLogin(int descriptor, Message message);
    void doRegister(int descriptor, Message message);
    void doCreate(int descriptor, Message message);
    void doOpen(int descriptor, Message message);
    void sendFileList(int descriptor);
    void saveFile(std::string filename);
    void sendFile(int descriptor, std::string filename, bool alreadyInMemory);
};

#endif // MYTCPSERVER_H
