#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QtSql>
#include "ConnectedEditor.h"
#include "ServerSocket.h"
#include "../common/Message.h"

class KangarooServer : public QObject
{
    Q_OBJECT
public:
    explicit KangarooServer(QObject *parent = nullptr);
    explicit KangarooServer(std::string address, unsigned short port);
    ~KangarooServer();

signals:

public slots:
    void newConnection();
    void incomingMessage(qintptr descriptor,Message message);
    void hostDisconnected(qintptr descriptor);

private:
    QSqlDatabase usersDB;
    QTcpServer *server;
    int guestId = 1;
    std::map<qintptr,ConnectedEditor> descriptorToEditor;
    std::map<QString, std::vector<qintptr>> filenameToDescriptors;
    std::map<QString, std::vector<Symbol>> filenameToSymbols;
    void modifyFileVector(const Message& m, std::vector<Symbol>& _symbols);
    //Temporary, for testing and debugging collaborative editing
    //std::vector<int> connectedList;
    void propagate(qintptr descriptor, Message message);
    void doLogin(qintptr descriptor, Message message);
    void doRegister(qintptr descriptor, Message message);
    void doCreate(qintptr descriptor, Message message);
    void doOpen(qintptr descriptor, Message message);
    void sendFileList(qintptr descriptor);
    void sendEditorList(qintptr descriptor, QString filename);
    void saveFile(QString filename);
    void sendFile(qintptr descriptor, QString filename, bool alreadyInMemory);
    void insertControlSymbols(qintptr descriptor, QString filename);
    void doOpenURI(qintptr descriptor, Message message);
    bool openDBConnection();
    void createUsersDB();
};

#endif // MYTCPSERVER_H
