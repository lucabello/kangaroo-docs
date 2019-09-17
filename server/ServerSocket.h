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
    qintptr getDescriptor();
    void disconnectFromHost();

signals:
    void signalMessage(qintptr, Message);
    void hostDisconnected(qintptr);

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readMessage();
    void writeMessage(Message message);


private:
    QTcpSocket *socket;
    qintptr descriptor;
};

#endif // MYTCPSOCKET_H
