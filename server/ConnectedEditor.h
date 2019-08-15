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
    int getDescriptor();
    ServerSocket* getSocket();
    QString getWorkingFile();
    void setSiteId(int id);
    void setWorkingFile(QString filename);

private:
    ServerSocket *mySocket;
    int descriptor;
    int siteId;
    QString workingFile;
};

#endif // CONNECTEDEDITOR_H
