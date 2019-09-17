#ifndef CONNECTEDEDITOR_H
#define CONNECTEDEDITOR_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QDebug>

class ServerSocket;

class ConnectedEditor
{
public:
    ConnectedEditor();
    ConnectedEditor(ServerSocket *s);
    ServerSocket* getSocket();
    qintptr getDescriptor();
    QString getUsername();
    int getSiteId();
    QString getWorkingFile();

    void setDescriptor(qintptr descr);
    void setUsername(QString user);
    void setSiteId(int id);
    void setWorkingFile(QString filename);

private:
    ServerSocket *mySocket;
    qintptr descriptor;
    QString username;
    int siteId;
    QString workingFile;
};

#endif // CONNECTEDEDITOR_H
