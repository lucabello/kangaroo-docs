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
    std::string getWorkingFile();
    void setSiteId(int id);
    void setWorkingFile(std::string filename);

private:
    ServerSocket *mySocket;
    int descriptor;
    int siteId;
    std::string workingFile;
};

#endif // CONNECTEDEDITOR_H
