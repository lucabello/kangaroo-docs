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
    explicit KangarooServer(QObject *parent = 0);
    explicit KangarooServer(std::string address, unsigned short port);

signals:

public slots:
    void newConnection();
    void control(int descriptor, std::string message);
    void action(int descriptor, Message m);

private:
    QTcpServer *server;
    std::map<int,ConnectedEditor> descriptorToEditor;
    std::map<std::string, std::vector<int>> filenameToDescriptors;
    //Temporary, for testing and debugging collaborative editing
    std::vector<int> connectedList;
};

#endif // MYTCPSERVER_H
