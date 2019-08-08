#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QDebug>
#include "../common/Message.h"
#include "../common/Symbol.h"

class ServerSocket : public QObject
{
    Q_OBJECT
public:
    explicit ServerSocket(QObject *parent = nullptr);
    explicit ServerSocket(QTcpSocket *s);
    int getDescriptor();

signals:
    void controlCommand(int, std::string);
    void actionCommand(int, Message);

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void writeData(char *data, int len);


private:
    QTcpSocket *socket;
    int descriptor;
    void processMessage(Message m);
};

#endif // MYTCPSOCKET_H
